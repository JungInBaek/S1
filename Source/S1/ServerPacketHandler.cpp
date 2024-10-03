#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "S1.h"
#include "S1GameInstance.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
    for (const Protocol::ObjectInfo& Player : pkt.players())
    {
        
    }

    // Lobby에서 캐릭터 선택 후 인덱스 전송
    Protocol::C_ENTER_GAME EnterGamePkt;
    EnterGamePkt.set_playerindex(0);
    SEND_PACKET(EnterGamePkt);

    return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
    // TODO: 조건문 수정
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSpawn(pkt);
    }

    return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        // TODO: 게임 종료 or 로비
    }

    return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSpawn(pkt);
    }

    return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleDespawn(pkt);
    }

    return true;
}

bool Handle_S_TURN(PacketSessionRef& session, Protocol::S_TURN& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleTurn(pkt);
    }

    return true;
}

bool Handle_S_JUMP(PacketSessionRef& session, Protocol::S_JUMP& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleJump(pkt);
    }

    return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleMove(pkt);
    }

    return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
    auto Msg = pkt.msg();

    return true;
}

bool Handle_S_FIRE(PacketSessionRef& session, Protocol::S_FIRE& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleFire(pkt);
    }
    return true;
}

bool Handle_S_SNIPER_FIRE(PacketSessionRef& session, Protocol::S_SNIPER_FIRE& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSniperFire(pkt);
    }
    return true;
}

bool Handle_S_CHANGE_ITEM(PacketSessionRef& session, Protocol::S_CHANGE_ITEM& pkt)
{
    if (auto* GameInstance = Cast<US1GameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleChangeItem(pkt);
    }
    return true;
}
