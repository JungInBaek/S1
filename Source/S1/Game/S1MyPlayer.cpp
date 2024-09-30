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
#include "Kismet/KismetMathLibrary.h"
#include "Bullet.h"
#include "S1.h"


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

		// Jumping
		EnhancedInputComponent->BindAction(ia_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(ia_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(ia_Move, ETriggerEvent::Triggered, this, &AS1MyPlayer::Move);
		EnhancedInputComponent->BindAction(ia_Move, ETriggerEvent::Completed, this, &AS1MyPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &AS1MyPlayer::LookUp);
		EnhancedInputComponent->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &AS1MyPlayer::Turn);

		// Gun Fire
		EnhancedInputComponent->BindAction(ia_Fire, ETriggerEvent::Started, this, &AS1MyPlayer::Fire);
	}
}

void AS1MyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 패킷 Send 판정
	bool forceSendPacket = false;
	if (lastInput != input)
	{
		forceSendPacket = true;
		lastInput = input;
	}

	// state 정보
	if (input == FVector2D::Zero())
	{
		SetMoveState(Protocol::MOVE_STATE_IDLE);
	}
	else
	{
		SetMoveState(Protocol::MOVE_STATE_RUN);
	}

	MovePacketSendTimer -= DeltaTime;

	if (forceSendPacket || MovePacketSendTimer <= 0)
	{
		MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

		Protocol::C_MOVE MovePkt;
		{
			Protocol::PosInfo* Info = MovePkt.mutable_info();
			Info->CopyFrom(*PlayerInfo);
			Info->set_yaw(yaw);
			Info->set_state(GetMoveState());
		}
		SEND_PACKET(MovePkt);
	}
}

void AS1MyPlayer::Move(const FInputActionValue& Value)
{
	input = Value.Get<FVector2D>();
	direction.X = input.X;
	direction.Y = input.Y;

	AddMovementInput(FTransform(GetActorRotation()).TransformVector(direction));
}

void AS1MyPlayer::LookUp(const FInputActionValue& Value)
{
	float value = Value.Get<float>();
	value *= GetWorld()->GetDeltaSeconds();
	value *= rate;
	AddControllerPitchInput(value);
}

void AS1MyPlayer::Turn(const FInputActionValue& Value)
{
	float value = Value.Get<float>();
	value *= GetWorld()->GetDeltaSeconds();
	value *= rate;
	AddControllerYawInput(value);

	yaw = GetControlRotation().Yaw;
	/*const uint64 objectId = PlayerInfo->object_id();

	{
		Protocol::C_TURN turnPkt;
		turnPkt.set_object_id(objectId);
		turnPkt.set_yaw(yaw);
		SEND_PACKET(turnPkt);
	}*/
}

void AS1MyPlayer::Fire(const FInputActionValue& Value)
{
	FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);

	Protocol::C_FIRE firePkt;
	SEND_PACKET(firePkt);
}
