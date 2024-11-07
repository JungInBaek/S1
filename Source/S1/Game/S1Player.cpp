// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/S1Player.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "S1MyPlayer.h"
#include "Bullet.h"
#include "S1.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnermyFSM.h"
#include "PlayerAnim.h"


AS1Player::AS1Player()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}

	// Set size for collision capsule
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->bIgnoreBaseRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	//GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (TempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));
	}

	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	if (TempSniperMesh.Succeeded())
	{
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		sniperGunComp->SetRelativeLocation(FVector(-42, 7, 1));
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}

	ObjectInfo = new Protocol::ObjectInfo();
	CurrentInfo = new Protocol::PosInfo();
	LastInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();

	change2();
	changeItemKey[1] = [this]() { change1(); };
	changeItemKey[2] = [this]() { change2(); };
}

AS1Player::~AS1Player()
{
	delete ObjectInfo;
	delete CurrentInfo;
	delete LastInfo;
	delete DestInfo;
	ObjectInfo = nullptr;
	CurrentInfo = nullptr;
	LastInfo = nullptr;
	DestInfo = nullptr;
}

void AS1Player::BeginPlay()
{
	Super::BeginPlay();

	{
		FVector Location = GetActorLocation();
		Protocol::VectorInfo* vectorInfo = DestInfo->mutable_vector_info();
		vectorInfo->set_x(Location.X);
		vectorInfo->set_y(Location.Y);
		vectorInfo->set_z(Location.Z);
		DestInfo->set_yaw(GetControlRotation().Yaw);

		SetState(Protocol::PLAYER_STATE_IDLE);
	}
}

void AS1Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void AS1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	{
		LastInfo->CopyFrom(*CurrentInfo);

		FVector Location = GetActorLocation();
		Protocol::VectorInfo vectorInfo = CurrentInfo->vector_info();
		vectorInfo.set_x(Location.X);
		vectorInfo.set_y(Location.Y);
		vectorInfo.set_z(Location.Z);
		CurrentInfo->mutable_vector_info()->CopyFrom(vectorInfo);
		CurrentInfo->set_yaw(GetControlRotation().Yaw);
	}

	if (IsMyPlayer())
	{
		return;
	}

	PlayerMoveTick(DeltaTime);
}

void AS1Player::PlayerMoveTick(float DeltaTime)
{
	SetActorRotation(FRotator(0, DestInfo->yaw(), 0));

	const Protocol::PlayerState state = GetState();
	//if (state == Protocol::MOVE_STATE_RUN)
	{
		Protocol::VectorInfo vectorInfo = DestInfo->vector_info();
		FVector location = GetActorLocation();
		FVector destLocation = FVector(vectorInfo.x(), vectorInfo.y(), vectorInfo.z());

		FVector direction = destLocation - location;
		const float distanceToDest = direction.Length();
		direction.Normalize();

		float speed = walkSpeed;
		float distance = (direction * speed * DeltaTime).Length();

		distance = FMath::Min(distanceToDest, distance);
		FVector nextLocation = location + direction * distance;
		nextLocation.Z = vectorInfo.z();

		SetActorLocation(nextLocation);
	}

	switch (state)
	{
	case Protocol::PLAYER_STATE_IDLE:
		PRINT_LOG(TEXT("IDLE"));
		break;
	case Protocol::PLAYER_STATE_FORWARD:
		PRINT_LOG(TEXT("FORWARD"));
		break;
	case Protocol::PLAYER_STATE_BACKWARD:
		PRINT_LOG(TEXT("BACKWARD"));
		break;
	case Protocol::PLAYER_STATE_RIGHT:
		PRINT_LOG(TEXT("RIGHT"));
		break;
	case Protocol::PLAYER_STATE_LEFT:
		PRINT_LOG(TEXT("LEFT"));
		break;
	case Protocol::PLAYER_STATE_RIGHT_FORWARD:
		PRINT_LOG(TEXT("RIGHT_FORWARD"));
		break;
	case Protocol::PLAYER_STATE_LEFT_FORWARD:
		PRINT_LOG(TEXT("LEFT_FORWARD"));
		break;
	case Protocol::PLAYER_STATE_RIGHT_BACKWARD:
		PRINT_LOG(TEXT("RIGHT_BACKWARD"));
		break;
	case Protocol::PLAYER_STATE_LEFT_BACKWARD:
		PRINT_LOG(TEXT("LEFT_BACKWARD"));
		break;
	case Protocol::PLAYER_STATE_JUMP:
		PRINT_LOG(TEXT("JUMP"));
		break;
	}
}

bool AS1Player::IsMyPlayer()
{
	return Cast<AS1MyPlayer>(this) != nullptr;
}

void AS1Player::Turn(float yaw)
{
	if (AController* controller = GetController())
	{
		auto rotator = GetActorRotation();
		rotator.Yaw = yaw;
		SetActorRotation(rotator);
	}
}

void AS1Player::Fire()
{
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	if (bUsingGrenadeGun)
	{
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
}

void AS1Player::SniperFire(const Protocol::S_SNIPER_FIRE& FirePkt)
{
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	if (bUsingGrenadeGun == false)
	{
		FVector startPos(FirePkt.start().x(), FirePkt.start().y(), FirePkt.start().z());
		FVector endPos(FirePkt.end().x(), FirePkt.end().y(), FirePkt.end().z());

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

			auto enermy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enermy)
			{
				auto enermyFSM = Cast<UEnermyFSM>(enermy);
				enermyFSM->OnDamageProcess();
			}
		}
	}
}

void AS1Player::Jump()
{
	Super::Jump();
}

void AS1Player::ChangeItem(uint8 key)
{
	changeItemKey[key]();
}

void AS1Player::change1()
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

void AS1Player::change2()
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void AS1Player::SetState(Protocol::PlayerState State)
{
	if (ObjectInfo->player_info().player_state() == State)
	{
		return;
	}

	Protocol::PlayerInfo playerInfo = ObjectInfo->player_info();
	playerInfo.set_player_state(State);
	ObjectInfo->mutable_player_info()->CopyFrom(playerInfo);

	// TODO
}

void AS1Player::SetObjectInfo(const Protocol::ObjectInfo& Info)
{
	if (ObjectInfo->object_id() != 0)
	{
		assert(ObjectInfo->object_id() == Info.object_id());
	}

	ObjectInfo->CopyFrom(Info);
}

void AS1Player::SetCurrentInfo(const Protocol::PosInfo& Info)
{
	CurrentInfo->CopyFrom(Info);
}

void AS1Player::SetDestInfo(const Protocol::PosInfo& Info)
{
	DestInfo->CopyFrom(Info);
}
