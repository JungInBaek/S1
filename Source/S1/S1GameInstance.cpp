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


void US1GameInstance::HandleSpawn(const Protocol::PlayerInfo& PlayerInfo, bool IsMine)
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
	const uint64 ObjectId = PlayerInfo.object_id();
	if (Players.Find(ObjectId) != nullptr)
	{
		return;
	}

	FVector SpawnLocation(PlayerInfo.x(), PlayerInfo.y(), PlayerInfo.z());

	if (IsMine)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		AS1Player* Player = Cast<AS1Player>(PC->GetPawn());
		if (Player == nullptr)
		{
			return;
		}

		Player->SetPlayerInfo(PlayerInfo);
		MyPlayer = Player;
		Players.Add(PlayerInfo.object_id(), Player);
	}
	else
	{
		AS1Player* Player = Cast<AS1Player>(World->SpawnActor(OtherPlayerClass, &SpawnLocation));
		Player->SetPlayerInfo(PlayerInfo);
		Players.Add(PlayerInfo.object_id(), Player);
	}
}

void US1GameInstance::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true);
}

void US1GameInstance::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (auto& Player : SpawnPkt.players())
	{
		HandleSpawn(Player, false);
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
}
