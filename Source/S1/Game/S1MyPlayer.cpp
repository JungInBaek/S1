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
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Bullet.h"
#include "S1.h"
#include "Enermy.h"
#include "EnermyFSM.h"
#include "PlayerAnim.h"


AS1MyPlayer::AS1MyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
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

	// 초기 속도 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(imc_default, 0);
		}
	}

	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	_crosshairUI->AddToViewport();

	ChangeToSniperGun(FInputActionValue());
}

//////////////////////////////////////////////////////////////////////////
// Input

void AS1MyPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(ia_Jump, ETriggerEvent::Started, this, &AS1MyPlayer::Jump);
		EnhancedInputComponent->BindAction(ia_Jump, ETriggerEvent::Completed, this, &AS1MyPlayer::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(ia_Move, ETriggerEvent::Triggered, this, &AS1MyPlayer::Move);
		EnhancedInputComponent->BindAction(ia_Move, ETriggerEvent::Completed, this, &AS1MyPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &AS1MyPlayer::LookUp);
		EnhancedInputComponent->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &AS1MyPlayer::Turn);

		// Gun Fire
		EnhancedInputComponent->BindAction(ia_Fire, ETriggerEvent::Started, this, &AS1MyPlayer::Fire);

		// Gun Change
		EnhancedInputComponent->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &AS1MyPlayer::ChangeToGrenadeGun);
		EnhancedInputComponent->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &AS1MyPlayer::ChangeToSniperGun);

		// Sniper Aim
		EnhancedInputComponent->BindAction(ia_Sniper, ETriggerEvent::Started, this, &AS1MyPlayer::SniperAim);
		EnhancedInputComponent->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &AS1MyPlayer::SniperAim);

		// Run
		EnhancedInputComponent->BindAction(ia_Run, ETriggerEvent::Started, this, &AS1MyPlayer::Run);
		EnhancedInputComponent->BindAction(ia_Run, ETriggerEvent::Completed, this, &AS1MyPlayer::Run);
	}
}

void AS1MyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 패킷 Send 판정
	bool forceSendPacket = false;
	if (lastInput != moveInput || LastInfo->vector_info().z() != CurrentInfo->vector_info().z())
	{
		forceSendPacket = true;
		lastInput = moveInput;
	}

	// state
	if (moveInput == FVector2D::Zero())
	{
		pState = Protocol::PLAYER_STATE_IDLE;
	}
	else
	{
		if (moveInput.X > 0.0 && moveInput.Y == 0.0)
		{
			pState = Protocol::PLAYER_STATE_FORWARD;
		}
		else if (moveInput.X < 0.0 && moveInput.Y == 0.0)
		{
			pState = Protocol::PLAYER_STATE_BACKWARD;
		}
		else if (moveInput.X == 0.0 && moveInput.Y > 0.0)
		{
			pState = Protocol::PLAYER_STATE_RIGHT;
		}
		else if (moveInput.X == 0.0 && moveInput.Y < 0.0)
		{
			pState = Protocol::PLAYER_STATE_LEFT;
		}
		else if (moveInput.X > 0.0 && moveInput.Y > 0.0)
		{
			pState = Protocol::PLAYER_STATE_RIGHT_FORWARD;
		}
		else if (moveInput.X > 0.0 && moveInput.Y < 0.0)
		{
			pState = Protocol::PLAYER_STATE_LEFT_FORWARD;
		}
		else if (moveInput.X < 0.0 && moveInput.Y > 0.0)
		{
			pState = Protocol::PLAYER_STATE_RIGHT_BACKWARD;
		}
		else if (moveInput.X < 0.0 && moveInput.Y < 0.0)
		{
			pState = Protocol::PLAYER_STATE_LEFT_BACKWARD;
		}
	}
	
	if (GetCharacterMovement()->IsFalling())
	{
		pState = Protocol::PLAYER_STATE_JUMP;
	}

	SetState(pState);

	MovePacketSendTimer -= DeltaTime;

	if (forceSendPacket || MovePacketSendTimer <= 0)
	{
		MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;
		{
			Protocol::C_MOVE MovePkt;
			{
				MovePkt.set_object_id(ObjectInfo->object_id());
				Protocol::PosInfo* Info = MovePkt.mutable_info();
				Info->CopyFrom(*CurrentInfo);
				Info->set_yaw(yaw);
			}
			SEND_PACKET(MovePkt);
		}
		{
			Protocol::C_STATE StatePkt;
			{
				StatePkt.set_object_id(ObjectInfo->object_id());
				StatePkt.set_player_state(GetState());
			}
			SEND_PACKET(StatePkt);
		}
	}
}

void AS1MyPlayer::Jump(const FInputActionValue& Value)
{
	Super::Jump();
}

void AS1MyPlayer::StopJumping(const FInputActionValue& Value)
{
	Super::StopJumping();
}

void AS1MyPlayer::Move(const FInputActionValue& Value)
{
	moveInput = Value.Get<FVector2D>();
	direction.X = moveInput.X;
	direction.Y = moveInput.Y;

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
}

void AS1MyPlayer::Fire(const FInputActionValue& Value)
{
	// 카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	if (bUsingGrenadeGun)
	{
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);

		Protocol::C_FIRE firePkt;
		SEND_PACKET(firePkt);
	}
	else
	{
		FVector startPos = FollowCamera->GetComponentLocation();
		FVector endPos = startPos + FollowCamera->GetForwardVector() * 5000;
		
		// LineTrace 충돌 정보 변수
		FHitResult hitInfo;

		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;

		// MyPlayer 충돌 제외
		params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		if (bHit)
		{
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// 충돌 물리 계산
			auto hitComp = hitInfo.GetComponent();
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 조준 방향
				FVector dir = (endPos - startPos).GetSafeNormal();

				// 날리는 힘(F = ma)
				FVector force = dir * hitComp->GetMass() * 50000;

				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			auto actor = hitInfo.GetActor()/*->GetDefaultSubobjectByName(TEXT("FSM"))*/;
			if (auto enermy = Cast<AEnermy>(actor))
			{
				/*auto enermyFSM = enermy->GetDefaultSubobjectByName(TEXT("FSM"));
				Cast<UEnermyFSM>(enermyFSM)->OnDamageProcess();*/

				Protocol::C_DAMAGE_ENERMY damagePkt;
				damagePkt.set_object_id(this->ObjectInfo->object_id());
				damagePkt.set_target_id(enermy->ObjectInfo->object_id());
				damagePkt.set_damage(50);
				SEND_PACKET(damagePkt);
			}
		}

		Protocol::C_SNIPER_FIRE firePkt;
		firePkt.set_object_id(ObjectInfo->object_id());

		Protocol::VectorInfo* start = firePkt.mutable_start();
		start->set_x(startPos.X);
		start->set_y(startPos.Y);
		start->set_z(startPos.Z);

		Protocol::VectorInfo* end = firePkt.mutable_end();
		end->set_x(endPos.X);
		end->set_y(endPos.Y);
		end->set_z(endPos.Z);

		SEND_PACKET(firePkt);
	}
}

void AS1MyPlayer::ChangeToGrenadeGun(const FInputActionValue& Value)
{
	if (bSniperAim == true)
	{
		bSniperAim = false;
		GetMesh()->SetVisibility(true);
		sniperGunComp->SetVisibility(true);
		_sniperUI->RemoveFromParent();
		FollowCamera->SetFieldOfView(90.0f);
	}
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);

	Protocol::C_CHANGE_ITEM changePkt;
	changePkt.set_object_id(ObjectInfo->object_id());
	changePkt.set_key(uint8(1));
	SEND_PACKET(changePkt);
}

void AS1MyPlayer::ChangeToSniperGun(const FInputActionValue& Value)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);

	Protocol::C_CHANGE_ITEM changePkt;
	changePkt.set_object_id(ObjectInfo->object_id());
	changePkt.set_key(uint8(2));
	SEND_PACKET(changePkt);
}

void AS1MyPlayer::SniperAim(const FInputActionValue& Value)
{
	if (bUsingGrenadeGun)
	{
		return;
	}

	if (bSniperAim == false)
	{
		bSniperAim = true;
		GetMesh()->SetVisibility(false);
		sniperGunComp->SetVisibility(false);
		_crosshairUI->RemoveFromParent();
		_sniperUI->AddToViewport();
		FollowCamera->SetFieldOfView(45.0f);
	}
	else
	{
		bSniperAim = false;
		GetMesh()->SetVisibility(true);
		sniperGunComp->SetVisibility(true);
		_crosshairUI->AddToViewport();
		_sniperUI->RemoveFromParent();
		FollowCamera->SetFieldOfView(90.0f);
	}
}

void AS1MyPlayer::Run(const FInputActionValue& Value)
{
	auto movement = GetCharacterMovement();
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}
