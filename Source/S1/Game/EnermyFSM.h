// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnermyFSM.generated.h"


UENUM(BlueprintType)
enum class EEnermyState : uint8
{
	None,
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S1_API UEnermyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnermyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();

public:
	void OnDamageProcess();

public:
	UPROPERTY()
	class AEnermy* me;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EEnermyState State = EEnermyState::Idle;

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 2;

	float currentTime = 0;

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class AS1Player* target;

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int32 hp = 100;

	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float dieSpeed = 50.0f;

	// 사용 중인 애니메이션 블루프린트
	UPROPERTY()
	class UEnermyAnim* anim;
};