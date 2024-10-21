// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Protocol.pb.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enermy.generated.h"


UCLASS()
class S1_API AEnermy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnermy();
	virtual ~AEnermy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


public:
	void SetObjectInfo(const Protocol::ObjectInfo& Info);
	void SetCurrentInfo(const Protocol::PosInfo& Info);
	void SetDestInfo(const Protocol::PosInfo& Info);
	void SetState(Protocol::EnermyState State);

	Protocol::ObjectInfo* GetObjectInfo() { return ObjectInfo; }
	Protocol::PosInfo* GetCurrentInfo() { return CurrentInfo; }
	Protocol::EnermyState GetState() { return ObjectInfo->enermy_info().enermy_state(); }

public:
	class Protocol::ObjectInfo* ObjectInfo;	// 현재 오브젝트 정보

	class Protocol::PosInfo* CurrentInfo;	// 현재 위치 정보
	class Protocol::PosInfo* DestInfo;		// 목적지 정보

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class UEnermyFSM* enermyFsm;
};
