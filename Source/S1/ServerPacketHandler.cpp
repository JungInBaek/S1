#include "ServerPacketHandler.h"
#include "BufferReader.h"


//PacketHandlerFunc GPacketHandler[UINT16_MAX];
//
//
//bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
//{
//    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
//
//    // TODO: Log
//
//    return false;
//}
//
//bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
//{
//    if (pkt.success() == false)
//    {
//        return false;
//    }
//
//    if (pkt.players().size() == 0)
//    {
//        // 캐릭터 생성창
//    }
//
//    // 입장 UI 버튼 눌러서 게임 입장
//    Protocol::C_ENTER_GAME enterGamePkt;
//    enterGamePkt.set_playerindex(0);    // 첫번째 캐릭터
//    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
//    session->Send(sendBuffer);
//
//    return true;
//}
//
//bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
//{
//    // TODO
//    return true;
//}
//
//bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
//{
//    std::cout << pkt.msg() << endl;
//
//
//    return true;
//}