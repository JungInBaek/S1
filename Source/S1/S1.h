// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(S1, Log, All);

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_CALLINFO() UE_LOG(S1, Warning, TEXT("%s"), *CALLINFO)
#define PRINT_LOG(fmt, ...) UE_LOG(S1, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))


struct PacketHeader
{
	uint16 size;
	uint16 id;
};


/*-----------------------
		SendBuffer
-----------------------*/

class SendBuffer : public TSharedFromThis<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer.GetData(); }
	int32 WriteSize() { return _writeSize; }
	int32 Capacity() { return static_cast<int32>(_buffer.Num()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);

private:
	TArray<BYTE> _buffer;
	int32 _writeSize = 0;
};


#define USING_SHARED_PTR(name) using name##Ref = TSharedPtr<class name>;

USING_SHARED_PTR(Session);
USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(SendBuffer);


#include "ServerPacketHandler.h"
#include "S1GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


#define SEND_PACKET(Pkt)															\
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Pkt);			\
	Cast<US1GameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);