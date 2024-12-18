#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "S1.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_LEAVE_GAME = 1004,
	PKT_S_LEAVE_GAME = 1005,
	PKT_S_SPAWN = 1006,
	PKT_S_DESPAWN = 1007,
	PKT_C_TURN = 1008,
	PKT_S_TURN = 1009,
	PKT_C_JUMP = 1010,
	PKT_S_JUMP = 1011,
	PKT_C_MOVE = 1012,
	PKT_S_MOVE = 1013,
	PKT_C_STATE = 1014,
	PKT_S_STATE = 1015,
	PKT_S_ENERMY_INFO = 1016,
	PKT_C_CHAT = 1017,
	PKT_S_CHAT = 1018,
	PKT_C_FIRE = 1019,
	PKT_S_FIRE = 1020,
	PKT_C_SNIPER_FIRE = 1021,
	PKT_S_SNIPER_FIRE = 1022,
	PKT_C_CHANGE_ITEM = 1023,
	PKT_S_CHANGE_ITEM = 1024,
	PKT_C_DAMAGE_ENERMY = 1025,
	PKT_S_ATTACK_ENERMY = 1026,
	PKT_S_ENERMY_DIE = 1027,
};

// Custom Handler
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt);
bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt);
bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt);
bool Handle_S_TURN(PacketSessionRef& session, Protocol::S_TURN& pkt);
bool Handle_S_JUMP(PacketSessionRef& session, Protocol::S_JUMP& pkt);
bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt);
bool Handle_S_STATE(PacketSessionRef& session, Protocol::S_STATE& pkt);
bool Handle_S_ENERMY_INFO(PacketSessionRef& session, Protocol::S_ENERMY_INFO& pkt);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt);
bool Handle_S_FIRE(PacketSessionRef& session, Protocol::S_FIRE& pkt);
bool Handle_S_SNIPER_FIRE(PacketSessionRef& session, Protocol::S_SNIPER_FIRE& pkt);
bool Handle_S_CHANGE_ITEM(PacketSessionRef& session, Protocol::S_CHANGE_ITEM& pkt);
bool Handle_S_ATTACK_ENERMY(PacketSessionRef& session, Protocol::S_ATTACK_ENERMY& pkt);
bool Handle_S_ENERMY_DIE(PacketSessionRef& session, Protocol::S_ENERMY_DIE& pkt);


class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LEAVE_GAME>(Handle_S_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SPAWN>(Handle_S_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DESPAWN>(Handle_S_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_TURN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_TURN>(Handle_S_TURN, session, buffer, len); };
		GPacketHandler[PKT_S_JUMP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_JUMP>(Handle_S_JUMP, session, buffer, len); };
		GPacketHandler[PKT_S_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MOVE>(Handle_S_MOVE, session, buffer, len); };
		GPacketHandler[PKT_S_STATE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STATE>(Handle_S_STATE, session, buffer, len); };
		GPacketHandler[PKT_S_ENERMY_INFO] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENERMY_INFO>(Handle_S_ENERMY_INFO, session, buffer, len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len); };
		GPacketHandler[PKT_S_FIRE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_FIRE>(Handle_S_FIRE, session, buffer, len); };
		GPacketHandler[PKT_S_SNIPER_FIRE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SNIPER_FIRE>(Handle_S_SNIPER_FIRE, session, buffer, len); };
		GPacketHandler[PKT_S_CHANGE_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHANGE_ITEM>(Handle_S_CHANGE_ITEM, session, buffer, len); };
		GPacketHandler[PKT_S_ATTACK_ENERMY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ATTACK_ENERMY>(Handle_S_ATTACK_ENERMY, session, buffer, len); };
		GPacketHandler[PKT_S_ENERMY_DIE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENERMY_DIE>(Handle_S_ENERMY_DIE, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_TURN& pkt) { return MakeSendBuffer(pkt, PKT_C_TURN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_JUMP& pkt) { return MakeSendBuffer(pkt, PKT_C_JUMP); }
	static SendBufferRef MakeSendBuffer(Protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_STATE& pkt) { return MakeSendBuffer(pkt, PKT_C_STATE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_FIRE& pkt) { return MakeSendBuffer(pkt, PKT_C_FIRE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SNIPER_FIRE& pkt) { return MakeSendBuffer(pkt, PKT_C_SNIPER_FIRE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHANGE_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_CHANGE_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DAMAGE_ENERMY& pkt) { return MakeSendBuffer(pkt, PKT_C_DAMAGE_ENERMY); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
		{
			return false;
		}

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
#endif

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
				
#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		pkt.SerializeToArray(&header[1], dataSize);
#else
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
#endif

		sendBuffer->Close(packetSize);
		return sendBuffer;
	}
};