// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class S1_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
	// 매 프레임 갱신되는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


public:
	// 플레이어 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float speed;

	// 플레이어 점프 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;
};
