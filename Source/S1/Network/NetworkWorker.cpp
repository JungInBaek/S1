// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "PacketSession.h"


RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session) : Socket(Socket), SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{
	
}

bool RecvWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
	return true;
}

uint32 RecvWorker::Run()
{
	while (Running)
	{
		TArray<uint8> Packet;
		if (ReceviePacket(OUT Packet) == false)
		{
			continue;
		}

		if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
		{
			Session->RecvPacketQueue.Enqueue(Packet);
		}
	}

	return 0;
}

void RecvWorker::Exit()
{

}

void RecvWorker::Destroy()
{
	Running = false;
}

bool RecvWorker::ReceviePacket(TArray<uint8>& OutPacket)
{
	// 패킷 헤더 파싱
	const int32 HeaderSize = sizeof(FPacketHeader);
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (RecevieDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
	{
		return false;
	}

	// ID, Size 추출
	FPacketHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		UE_LOG(LogTemp, Log, TEXT("Recv PacketID : %d, PacketSize : %d"), Header.PacketID, Header.PacketSize);
	}

	// 패킷 헤더 복사
	OutPacket = HeaderBuffer;

	// 패킷 내용 파싱
	TArray<uint8> PayloadBuffer;
	const int32 PayloadSize = Header.PacketSize - HeaderSize;
	OutPacket.AddZeroed(PayloadSize);

	if (RecevieDesiredBytes(&PayloadBuffer[HeaderSize], PayloadSize) == false)
	{
		return false;
	}

	return true;
}

bool RecvWorker::RecevieDesiredBytes(uint8* Results, int32 Size)
{
	uint32 PendingDataSize;
	if (Socket->HasPendingData(PendingDataSize) == false || PendingDataSize <= 0)
	{
		return false;
	}

	int32 Offset = 0;
	while (Size > 0)
	{
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, NumRead);
		check(NumRead <= Size);

		if (NumRead <= 0)
		{
			return false;
		}

		Offset += NumRead;
		Size -= NumRead;
	}

	return true;
}