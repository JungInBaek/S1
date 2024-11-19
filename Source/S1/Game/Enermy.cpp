// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Enermy.h"
#include "EnermyFSM.h"
#include "EnermyAnim.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnermy::AEnermy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
	}

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->bIgnoreBaseRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	ObjectInfo = new Protocol::ObjectInfo();
	CurrentInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();

	// EnermyFSM 컴포넌트
	enermyFsm = CreateDefaultSubobject<UEnermyFSM>(TEXT("FSM"));

	// 애니메이션 블루프린트
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/ABP_Enermy.ABP_Enermy_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}
}

AEnermy::~AEnermy()
{
	delete ObjectInfo;
	delete CurrentInfo;
	delete DestInfo;
	ObjectInfo = nullptr;
	CurrentInfo = nullptr;
	DestInfo = nullptr;
}

// Called when the game starts or when spawned
void AEnermy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnermy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnermy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnermy::SetObjectInfo(const Protocol::ObjectInfo& Info)
{
	if (ObjectInfo->object_id() != 0)
	{
		assert(ObjectInfo->object_id() == Info.object_id());
	}

	ObjectInfo->CopyFrom(Info);
}

void AEnermy::SetCurrentInfo(const Protocol::PosInfo& Info)
{
	CurrentInfo->CopyFrom(Info);
}

void AEnermy::SetDestInfo(const Protocol::PosInfo& Info)
{
	DestInfo->CopyFrom(Info);
}

void AEnermy::SetState(Protocol::EnermyState State)
{
	if (ObjectInfo->enermy_info().enermy_state() == State)
	{
		return;
	}

	Protocol::EnermyInfo enermyInfo = ObjectInfo->enermy_info();
	enermyInfo.set_enermy_state(State);
	ObjectInfo->mutable_enermy_info()->CopyFrom(enermyInfo);
	enermyFsm->State = static_cast<EEnermyState>(State);
}

void AEnermy::AttackEnermy(uint64 targetId)
{
	enermyFsm->anim->bAttackPlay = true;
}

