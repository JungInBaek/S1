// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnermyFSM.h"
#include "EnermyAnim.generated.h"

/**
 * 
 */
UCLASS()
class S1_API UEnermyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 공격 애니메이션 종료 이벤트 함수
	UFUNCTION(BlueprintCallable, category = "FSMEvent")
	void OnEndAttackAnimation();

	// 피격 애니메이션 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "FSMEvent")
	void PlayDamageAnim(FName sectionName);
	
public:
	// 상태 머신 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
	EEnermyState animState;

	// 공격 상태 재생 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
	bool bAttackPlay = false;
};
