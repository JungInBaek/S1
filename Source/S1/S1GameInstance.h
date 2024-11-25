// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "S1.h"
#include "S1GameInstance.generated.h"


class AS1Player;
class AEnermy;


UCLASS()
class S1_API US1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BluePrintCallable)
	void ConnectToGameServer();

	UFUNCTION(BluePrintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

public:
	void HandleSpawnPlayer(const Protocol::ObjectInfo& ObjectInfo, bool IsMine);
	void HandleSpawnPlayer(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawnEnermy(const Protocol::ObjectInfo& ObjectInfo);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);
	void HandleState(const Protocol::S_STATE& StatePkt);
	void HandleEnermyInfo(const Protocol::S_ENERMY_INFO& EnermyPkt);
	void HandleTurn(const Protocol::S_TURN& TurnPkt);
	void HandleMove(const Protocol::S_MOVE& MovePkt);
	void HandleJump(const Protocol::S_JUMP& JumpPkt);
	void HandleFire(const Protocol::S_FIRE& FirePkt);
	void HandleSniperFire(const Protocol::S_SNIPER_FIRE& FirePkt);
	void HandleChangeItem(const Protocol::S_CHANGE_ITEM& ChangePkt);
	void HandleAttackEnermy(const Protocol::S_ATTACK_ENERMY& AttackPkt);
	void HandleDespawn(uint64 ObjectId);
	void HandlePlayerState(const Protocol::S_STATE& StatePkt);
	void HandleEnermyState(const Protocol::S_STATE& StatePkt);
	void HandleEnermyDie(const Protocol::S_ENERMY_DIE& DiePkt);

public:
	// Game Server
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<class PacketSession> GameServerSession;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AS1Player> OtherPlayerClass;

	AS1Player* MyPlayer;
	TMap<uint64, AS1Player*> Players;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnermy> EnermyClass;

	TMap<uint64, AEnermy*> Enermies;
};