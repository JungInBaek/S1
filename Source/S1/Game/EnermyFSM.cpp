// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/EnermyFSM.h"
#include "S1Player.h"
#include "Enermy.h"
#include "Kismet/GameplayStatics.h"
#include "S1.h"
#include "Components/CapsuleComponent.h"


// Sets default values for this component's properties
UEnermyFSM::UEnermyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnermyFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<AEnermy>(GetOwner());
}


// Called every frame
void UEnermyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (target == nullptr)
	{
		float min = FLT_MAX;
		TArray<AActor*, FDefaultAllocator> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AS1Player::StaticClass(), actors);
		for (AActor* actor : actors)
		{
			AS1Player* player = Cast<AS1Player>(actor);
			float distance = FVector::Distance(player->GetActorLocation(), me->GetActorLocation());
			if (min > distance)
			{
				min = distance;
				target = player;
			}
		}
	}

	// 실행창에 상태 메시지 출력
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (mState)
	{
	case EEnermyState::Idle:
		IdleState();
		break;
	case EEnermyState::Move:
		MoveState();
		break;
	case EEnermyState::Attack:
		AttackState();
		break;
	case EEnermyState::Damage:
		DamageState();
		break;
	case EEnermyState::Die:
		DieState();
		break;
	}
}

void UEnermyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime >= idleDelayTime)
	{
		mState = EEnermyState::Move;
		currentTime = 0;
	}
}

void UEnermyFSM::MoveState()
{
	FVector destLocation = target->GetActorLocation();
	FVector dir = destLocation - me->GetActorLocation();
	me->AddMovementInput(dir.GetSafeNormal());

	if (dir.Size() <= attackRange)
	{
		mState = EEnermyState::Attack;
	}
}

void UEnermyFSM::AttackState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime >= attackDelayTime)
	{
		PRINT_LOG(TEXT("Attack!"));
		currentTime = 0;
	}

	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange)
	{
		mState = EEnermyState::Move;
	}
}

void UEnermyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		mState = EEnermyState::Idle;
		currentTime = 0;
	}
}

void UEnermyFSM::DieState()
{
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	if (P.Z < -200.0f)
	{
		me->Destroy();
	}
}

void UEnermyFSM::OnDamageProcess()
{
	hp -= 50;

	if (hp > 0)
	{
		mState = EEnermyState::Damage;
	}
	else
	{
		mState = EEnermyState::Die;
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}