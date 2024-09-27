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


AS1Player::AS1Player()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	/*GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;*/

	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (TempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}



	PlayerInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();
}

AS1Player::~AS1Player()
{
	delete PlayerInfo;
	delete DestInfo;
	PlayerInfo = nullptr;
	DestInfo = nullptr;
}

void AS1Player::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	{
		FVector Location = GetActorLocation();
		DestInfo->set_x(Location.X);
		DestInfo->set_y(Location.Y);
		DestInfo->set_z(Location.Z);
		DestInfo->set_yaw(GetControlRotation().Yaw);

		SetMoveState(Protocol::MOVE_STATE_IDLE);
	}
}

void AS1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	{
		FVector Location = GetActorLocation();
		PlayerInfo->set_x(Location.X);
		PlayerInfo->set_y(Location.Y);
		PlayerInfo->set_z(Location.Z);
		PlayerInfo->set_yaw(GetControlRotation().Yaw);
	}

	if (IsMyPlayer())
	{
		return;
	}

	SetActorRotation(FRotator(0, DestInfo->yaw(), 0));
	SetActorLocation(FVector(DestInfo->x(), DestInfo->y(), DestInfo->z()));
	/*AddActorLocalRotation(FRotator(0, DestInfo->yaw(), 0));
	SetDestInfo(Protocol::PosInfo());*/

	//FVector Location = GetActorLocation();
	//FVector DestLocation = FVector(DestInfo->x(), DestInfo->y(), DestInfo->z());

	//// 방향
	//FVector MoveDir = DestLocation - Location;
	//const float DistToDest = MoveDir.Length();
	//MoveDir.Normalize();

	//// 거리
	//float MoveDist = (MoveDir * 600.f * DeltaTime).Length();
	//MoveDist = FMath::Min(MoveDist, DistToDest);

	//FVector NextLocation = Location + MoveDir * MoveDist;
	//SetActorLocation(NextLocation);

	//const Protocol::MoveState State = PlayerInfo->state();
	//if (State == Protocol::MOVE_STATE_RUN)
	//{
	//	SetActorRotation(FRotator(0, DestInfo->yaw(), 0));

	//	// P = P0 + vt;
	//	FVector P0 = GetActorLocation();
	//	FVector dest = FVector(DestInfo->x(), DestInfo->y(), DestInfo->z());

	//	// 방향
	//	FVector dir = dest - P0;
	//	const float destDist = dir.Length();
	//	dir.Normalize();

	//	// 거리
	//	float dist = (dir * 600.f * DeltaTime).Length();
	//	dist = FMath::Min(dist, destDist);

	//	FVector vt = dir * dist;
	//	FVector P = P0 + vt;

	//	SetActorLocation(P);
	//	//AddMovementInput(GetActorForwardVector());
	//}
	//else
	//{
	//	// TODO: 보정
	//}
}

bool AS1Player::IsMyPlayer()
{
	return Cast<AS1MyPlayer>(this) != nullptr;
}

void AS1Player::Fire()
{
	FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
}

void AS1Player::SetMoveState(Protocol::MoveState State)
{
	if (PlayerInfo->state() == State)
	{
		return;
	}

	PlayerInfo->set_state(State);

	// TODO
}

void AS1Player::SetPlayerInfo(const Protocol::PosInfo& Info)
{
	if (PlayerInfo->object_id() != 0)
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}

	PlayerInfo->CopyFrom(Info);

	FVector Location(Info.x(), Info.y(), Info.z());
	SetActorLocation(Location);
}

void AS1Player::SetDestInfo(const Protocol::PosInfo& Info)
{
	if (PlayerInfo->object_id() != 0)
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}

	DestInfo->CopyFrom(Info);

	SetMoveState(Info.state());
}