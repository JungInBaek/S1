// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "S1Player.h"
#include "GameFramework/CharacterMovementComponent.h"


void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();
	if (AS1Player* player = Cast<AS1Player>(ownerPawn))
	{
		FVector velocityVector = player->GetVelocity();
		FVector forwardVector = player->GetActorForwardVector();
		speed = FVector::DotProduct(forwardVector, velocityVector);

		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}