// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/EnermyFSM.h"
#include "S1Player.h"
#include "Enermy.h"
#include "Kismet/GameplayStatics.h"
#include "S1.h"
#include "Components/CapsuleComponent.h"
#include "EnermyAnim.h"


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

	// 소유 객체
	me = Cast<AEnermy>(GetOwner());

	anim = Cast<UEnermyAnim>(me->GetMesh()->GetAnimInstance());
}


// Called every frame
void UEnermyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 메시지 출력
	FString logMsg = UEnum::GetValueAsString(State);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (State)
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
	/*currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime >= idleDelayTime)
	{
		mState = EEnermyState::Move;
		currentTime = 0;
	}*/
}

void UEnermyFSM::MoveState()
{
	// 목적지 벡터
	Protocol::VectorInfo destVector = me->DestInfo->vector_info();
	FVector location = me->GetActorLocation();
	FVector destLocation = FVector(destVector.x(), destVector.y(), destVector.z());

	// 목적지 방향을 계산
	FVector direction = destLocation - location;
	const float distanceToDest = direction.Length();
	direction.Z = 0; // Z축 회전을 무시
	direction.Normalize();

	// 방향 벡터로부터 yaw 회전을 생성
	FRotator currentRotation = me->GetActorRotation();
	FRotator targetRotation = direction.Rotation();
	targetRotation.Pitch = currentRotation.Pitch; // Pitch 유지
	targetRotation.Roll = currentRotation.Roll;   // Roll 유지

	// Actor를 목표 회전으로 부드럽게 회전
	float rotationSpeed = 5.0f; // 회전 속도 조절
	FRotator newRotation = FMath::RInterpTo(currentRotation, targetRotation, GetWorld()->DeltaTimeSeconds, rotationSpeed);
	me->SetActorRotation(targetRotation);


	float distance = (direction * 500.f * GetWorld()->DeltaTimeSeconds).Length();
	distance = FMath::Min(distanceToDest, distance);

	FVector nextLocation = location + direction * distance;
	nextLocation.Z = destVector.z();
	me->SetActorLocation(nextLocation);
}

void UEnermyFSM::AttackState()
{
	/*currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime >= attackDelayTime)
	{
		PRINT_LOG(TEXT("Attack!"));
		currentTime = 0;
	}

	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange)
	{
		mState = EEnermyState::Move;
	}*/

	//anim->bAttackPlay = true;
}

void UEnermyFSM::DamageState()
{
	/*currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		mState = EEnermyState::Idle;
		currentTime = 0;
	}*/
}

void UEnermyFSM::DieState()
{
	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	/*hp -= 50;

	if (hp > 0)
	{
		State = EEnermyState::Damage;
	}*/
	//if (hp <= 0)
	//{
	//	//State = EEnermyState::Die;
	//	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
}