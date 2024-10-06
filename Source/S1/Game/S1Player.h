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
	void SniperFire(const Protocol::S_SNIPER_FIRE& FirePkt);
	void PlayerMoveTick(float DeltaTime);
	void Jump();
	void ChangeItem(uint8 key);

public:
	void SetObjectInfo(const Protocol::PosInfo& Info);
	void SetMoveState(Protocol::MoveState State);

	void SetDestInfo(const Protocol::PosInfo& Info);

	Protocol::PosInfo* GetPlayerInfo() { return ObjectInfo; }
	Protocol::MoveState GetMoveState() { return ObjectInfo->state(); }

protected:
	class Protocol::PosInfo* LastInfo;		// 지난 오브젝트 정보
	class Protocol::PosInfo* ObjectInfo;	// 현재 오브젝트 정보
	class Protocol::PosInfo* DestInfo;		// 목적지

public:
	float rate = 45.0f;

public:
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;

	UPROPERTY(EditAnywhere, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

public:
	std::function<void()> changeItemKey[UINT8_MAX];
	void change1();
	void change2();

	bool bUsingGrenadeGun = true;
	bool bSniperAim = false;
};
