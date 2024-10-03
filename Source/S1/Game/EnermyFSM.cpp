// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/EnermyFSM.h"

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

	// ...
	
}


// Called every frame
void UEnermyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
}

void UEnermyFSM::MoveState()
{
}

void UEnermyFSM::AttackState()
{
}

void UEnermyFSM::DamageState()
{
}

void UEnermyFSM::DieState()
{
}

