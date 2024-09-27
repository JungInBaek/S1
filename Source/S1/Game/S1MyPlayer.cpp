// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/S1MyPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "S1.h"
#include "Kismet/KismetMathLibrary.h"


AS1MyPlayer::AS1MyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0, 70, 90));
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationYaw = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

}

void AS1MyPlayer::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(imc_default, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AS1MyPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(ia_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(ia_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(ia_Move, ETriggerEvent::Triggered, this, &AS1MyPlayer::Move);
		//EnhancedInputComponent->BindAction(ia_Move, ETriggerEvent::Completed, this, &AS1MyPlayer::Move);

		//Looking
		EnhancedInputComponent->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &AS1MyPlayer::LookUp);
		EnhancedInputComponent->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &AS1MyPlayer::Turn);
	}
}

void AS1MyPlayer::Tick(float DeltaTime)
{
	PlayerMove();

	Super::Tick(DeltaTime);

	// 현재 위치, 회전 정보 전송
	Protocol::C_MOVE MovePkt;
	{
		Protocol::PosInfo* Info = MovePkt.mutable_info();
		Info->CopyFrom(*PlayerInfo);
		Info->set_yaw(GetActorRotation().Yaw);
		Info->set_state(GetMoveState());
	}
	SEND_PACKET(MovePkt);

	// Send 판정
	//bool ForceSendPacket = false;
	//if (LastDesiredInput != DesiredInput)
	//{
	//	ForceSendPacket = true;
	//	LastDesiredInput = DesiredInput;
	//}
	//if (LastDesiredYaw != DesiredYaw)
	//{
	//	ForceSendPacket = true;
	//	LastDesiredYaw = DesiredYaw;
	//}

	//// State 정보
	//if (DesiredInput == FVector2D::Zero())
	//{
	//	PlayerInfo->set_state(Protocol::MOVE_STATE_IDLE);
	//}
	//else
	//{
	//	PlayerInfo->set_state(Protocol::MOVE_STATE_RUN);
	//}

	//MovePacketSendTimer -= DeltaTime;
	//if (MovePacketSendTimer <= 0 || ForceSendPacket)
	//{
	//	MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

	//	Protocol::C_MOVE MovePkt;

	//	// 현재 위치 정보
	//	{
	//		Protocol::PosInfo* Info = MovePkt.mutable_info();
	//		Info->CopyFrom(*PlayerInfo);
	//		Info->set_yaw(DesiredYaw);
	//		Info->set_state(GetMoveState());
	//	}

	//	SEND_PACKET(MovePkt);
	//}
}

void AS1MyPlayer::PlayerMove()
{
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void AS1MyPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	direction.X = MovementVector.X;
	direction.Y = MovementVector.Y;

	/*FVector P0 = GetActorLocation();
	FVector vt = DesiredMoveDirection * walkSpeed * DeltaTime;
	FVector P = P0 + vt;*/

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		//const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		/*AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);*/

		// Cache
		{
			// 키보드 입력
			//DesiredInput = MovementVector;

			// 방향 벡터
			desiredDirection = FVector::ZeroVector;
			/*DesiredMoveDirection += ForwardDirection * MovementVector.Y;
			DesiredMoveDirection += RightDirection * MovementVector.X;*/
			//DesiredMoveDirection.Normalize();

			//const FVector Location = GetActorLocation();
			//FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
			//DesiredYaw = Rotator.Yaw;
			//DesiredYaw = Rotation.Yaw;
		}
	}
}

void AS1MyPlayer::LookUp(const FInputActionValue& Value)
{
	float value = Value.Get<float>();
	AddControllerPitchInput(value);
}

void AS1MyPlayer::Turn(const FInputActionValue& Value)
{
	float value = Value.Get<float>();
	AddControllerYawInput(value);
	desiredYaw = value;
}