// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Protocol.pb.h"
#include "S1Player.generated.h"


UCLASS()
class S1_API AS1Player : public ACharacter
{
	GENERATED_BODY()

public:
	AS1Player();
	virtual ~AS1Player();

protected:
	// To add mapping context
	virtual void BeginPlay();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

public:
	bool IsMyPlayer();

	void Turn(float yaw);
	void Fire();
	void PlayerMove(float DeltaTime);

public:
	void SetPlayerInfo(const Protocol::PosInfo& Info);
	void SetMoveState(Protocol::MoveState State);

	void SetDestInfo(const Protocol::PosInfo& Info);

	Protocol::PosInfo* GetPlayerInfo() { return PlayerInfo; }
	Protocol::MoveState GetMoveState() { return PlayerInfo->state(); }

protected:
	class Protocol::PosInfo* PlayerInfo;	// 현재 플레이어 정보
	class Protocol::PosInfo* DestInfo;		// 목적지

public:
	float rate = 45.0f;

public:
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;
};
