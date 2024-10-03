// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GameInstance.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "S1MyPlayer.h"
#include "Enermy.h"
#include "S1.h"


void US1GameInstance::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	bool Connected = Socket->Connect(*InternetAddr);
	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		// Session
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();

		// Lobby
		{
			Protocol::C_LOGIN Pkt;
			SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Pkt);
			SendPacket(SendBuffer);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void US1GameInstance::DisconnectFromGameServer()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	Protocol::C_LEAVE_GAME LeavePkt;
	SEND_PACKET(LeavePkt);
}

void US1GameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->HandleRecvPackets();
}

void US1GameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->SendPacket(SendBuffer);
}


void US1GameInstance::HandleSpawnPlayer(const Protocol::ObjectInfo& ObjectInfo, bool IsMine)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// 중복 처리 체크
	const uint64 ObjectId = ObjectInfo.object_id();
	if (Players.Find(ObjectId) != nullptr)
	{
		return;
	}

	const Protocol::PosInfo& PosInfo = ObjectInfo.pos_info();
	FVector SpawnLocation(PosInfo.vector_info().x(), PosInfo.vector_info().y(), PosInfo.vector_info().z());

	if (IsMine)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		AS1Player* Player = Cast<AS1Player>(PC->GetPawn());
		if (Player == nullptr)
		{
			return;
		}

		Player->SetObjectInfo(PosInfo);
		MyPlayer = Player;
		Players.Add(ObjectInfo.object_id(), Player);
	}
	else
	{
		if (AS1Player* Player = Cast<AS1Player>(World->SpawnActor(OtherPlayerClass, &SpawnLocation)))
		{
			Player->SetObjectInfo(PosInfo);
			Players.Add(ObjectInfo.object_id(), Player);
		}
	}
}

void US1GameInstance::HandleSpawnPlayer(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawnPlayer(EnterGamePkt.player(), true);
}

void US1GameInstance::HandleSpawnEnermy(const Protocol::ObjectInfo& ObjectInfo)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// 중복 처리 체크
	const uint64 ObjectId = ObjectInfo.object_id();
	if (Players.Find(ObjectId) != nullptr)
	{
		return;
	}

	const Protocol::PosInfo& PosInfo = ObjectInfo.pos_info();
	FVector SpawnLocation(PosInfo.vector_info().x(), PosInfo.vector_info().y(), PosInfo.vector_info().z());
	if (AEnermy* Enermy = Cast<AEnermy>(World->SpawnActor(EnermyClass, &SpawnLocation)))
	{
		Enermy->SetObjectInfo(PosInfo);
		Enermys.Add(ObjectInfo.object_id(), Enermy);
	}
}

void US1GameInstance::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (auto& object : SpawnPkt.objects())
	{
		Protocol::ObjectType objectType = object.object_type();
		switch (objectType)
		{
		case Protocol::OBJECT_TYPE_CREATURE:
			Protocol::CreatureType creatureType = object.creature_type();
			switch (creatureType)
			{
			case Protocol::CREATURE_TYPE_PLAYER:
				HandleSpawnPlayer(object, false);
				break;
			case Protocol::CREATURE_TYPE_NPC:
				break;
			case Protocol::CREATURE_TYPE_MONSTER:
				break;
			case Protocol::CREATURE_TYPE_ENERMY:
				HandleSpawnEnermy(object);
			}
			break;
		}
	}
}

void US1GameInstance::HandleDespawn(uint64 ObjectId)
{
	AS1Player** FindPlayer = Players.Find(ObjectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	World->DestroyActor(*FindPlayer);
	
	//Players.Remove(ObjectId);
}

void US1GameInstance::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (const uint64& ObjectId : DespawnPkt.object_ids())
	{
		HandleDespawn(ObjectId);
	}
}

void US1GameInstance::HandleTurn(const Protocol::S_TURN& TurnPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const uint64 objectId = TurnPkt.object_id();
	AS1Player** FindPlayer = Players.Find(objectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	AS1Player* player = *FindPlayer;
	if (player->IsMyPlayer())
	{
		return;
	}

	float yaw = TurnPkt.yaw();
	player->Turn(yaw);
}

void US1GameInstance::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const uint64 ObjectId = MovePkt.info().object_id();
	AS1Player** FindPlayer = Players.Find(ObjectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	AS1Player* Player = *FindPlayer;
	if (Player->IsMyPlayer())
	{
		return;
	}

	Player->SetDestInfo(MovePkt.info());
	/*Player->direction.X = MovePkt.info().x();
	Player->direction.Y = MovePkt.info().y();*/
}

void US1GameInstance::HandleJump(const Protocol::S_JUMP& JumpPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	const uint64 objectId = JumpPkt.object_id();
	AS1Player** FindPlayer = Players.Find(objectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	AS1Player* player = *FindPlayer;
	if (player->IsMyPlayer())
	{
		return;
	}

	//player->Jump();
}

void US1GameInstance::HandleFire(const Protocol::S_FIRE& FirePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	const uint64 objectId = FirePkt.object_id();
	AS1Player** FindPlayer = Players.Find(objectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	AS1Player* player = *FindPlayer;
	if (player->IsMyPlayer())
	{
		return;
	}

	player->Fire();
}

void US1GameInstance::HandleSniperFire(const Protocol::S_SNIPER_FIRE& FirePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	const uint64 objectId = FirePkt.object_id();
	AS1Player** FindPlayer = Players.Find(objectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	AS1Player* player = *FindPlayer;
	if (player->IsMyPlayer())
	{
		return;
	}

	player->SniperFire(FirePkt);
}

void US1GameInstance::HandleChangeItem(const Protocol::S_CHANGE_ITEM& ChangePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	const uint64 objectId = ChangePkt.object_id();
	AS1Player** FindPlayer = Players.Find(objectId);
	if (FindPlayer == nullptr)
	{
		return;
	}

	AS1Player* player = *FindPlayer;
	if (player->IsMyPlayer())
	{
		return;
	}

	player->ChangeItem(ChangePkt.key());
}
