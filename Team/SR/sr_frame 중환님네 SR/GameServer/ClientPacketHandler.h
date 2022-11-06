#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_S_OTHER_PLAYER = 1004,
	PKT_C_PLAYER_INPUT = 1005,
	PKT_S_PLAYER_INPUT = 1006,
	PKT_C_PLAYER_WORLD = 1007,
	PKT_S_PLAYER_WORLD = 1008,
	PKT_C_PLAYER_YAW_ACTION = 1009,
	PKT_S_PLAYER_YAW_ACTION = 1010,
	PKT_C_PLAYER_ACTION = 1011,
	PKT_S_PLAYER_ACTION = 1012,
	PKT_C_PLAYER_ARROW = 1013,
	PKT_S_PLAYER_ARROW = 1014,
	PKT_C_PLAYER_EQUIP = 1015,
	PKT_S_PLAYER_EQUIP = 1016,
	PKT_S_ALL_PLAYER_ENTER = 1017,
	PKT_C_PLAYER_DEAD = 1018,
	PKT_S_PLAYER_DEAD = 1019,
	PKT_C_PLAYER_RESPANW = 1020,
	PKT_S_PLAYER_RESPAWN = 1021,
	PKT_C_PLAYER_MOVE_STAGE = 1022,
	PKT_S_PLAYER_MOVE_STAGE = 1023,
	PKT_C_PLAYER_MOVE_STAGE_FINISH = 1024,
	PKT_S_PLAYER_MOVE_STAGE_FINISH = 1025,
	PKT_C_SPAWN_MONSTER = 1026,
	PKT_S_SPAWN_MONSTER = 1027,
	PKT_C_MONSTER_SET_TARGET = 1028,
	PKT_S_MONSTER_SET_TARGET = 1029,
	PKT_C_MONSTER_WORLD = 1030,
	PKT_S_MONSTER_WORLD = 1031,
	PKT_C_BOSS_SPAWN = 1032,
	PKT_S_BOSS_SPAWN = 1033,
	PKT_C_BOSS_WORLD = 1034,
	PKT_S_BOSS_WORLD = 1035,
	PKT_C_SATON_ATTACK = 1036,
	PKT_S_SATON_ATTACK = 1037,
	PKT_C_KOUKU_ATTACK = 1038,
	PKT_S_KOUKU_ATTACK = 1039,
	PKT_C_KOUKU_COUNTER = 1040,
	PKT_S_KOUKU_COUNTER = 1041,
	PKT_C_DEBUG_PKT = 1042,
	PKT_C_KOUKU_DAMAGE = 1043,
	PKT_C_KOUKU_RESULT = 1044,
	PKT_S_KOUKU_RESULT = 1045,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt);
bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt);
bool Handle_C_PLAYER_INPUT(PacketSessionRef& session, Protocol::C_PLAYER_INPUT& pkt);
bool Handle_C_PLAYER_WORLD(PacketSessionRef& session, Protocol::C_PLAYER_WORLD& pkt);
bool Handle_C_PLAYER_YAW_ACTION(PacketSessionRef& session, Protocol::C_PLAYER_YAW_ACTION& pkt);
bool Handle_C_PLAYER_ACTION(PacketSessionRef& session, Protocol::C_PLAYER_ACTION& pkt);
bool Handle_C_PLAYER_ARROW(PacketSessionRef& session, Protocol::C_PLAYER_ARROW& pkt);
bool Handle_C_PLAYER_EQUIP(PacketSessionRef& session, Protocol::C_PLAYER_EQUIP& pkt);
bool Handle_C_PLAYER_DEAD(PacketSessionRef& session, Protocol::C_PLAYER_DEAD& pkt);
bool Handle_C_PLAYER_RESPANW(PacketSessionRef& session, Protocol::C_PLAYER_RESPANW& pkt);
bool Handle_C_PLAYER_MOVE_STAGE(PacketSessionRef& session, Protocol::C_PLAYER_MOVE_STAGE& pkt);
bool Handle_C_PLAYER_MOVE_STAGE_FINISH(PacketSessionRef& session, Protocol::C_PLAYER_MOVE_STAGE_FINISH& pkt);
bool Handle_C_SPAWN_MONSTER(PacketSessionRef& session, Protocol::C_SPAWN_MONSTER& pkt);
bool Handle_C_MONSTER_SET_TARGET(PacketSessionRef& session, Protocol::C_MONSTER_SET_TARGET& pkt);
bool Handle_C_MONSTER_WORLD(PacketSessionRef& session, Protocol::C_MONSTER_WORLD& pkt);
bool Handle_C_BOSS_SPAWN(PacketSessionRef& session, Protocol::C_BOSS_SPAWN& pkt);
bool Handle_C_BOSS_WORLD(PacketSessionRef& session, Protocol::C_BOSS_WORLD& pkt);
bool Handle_C_SATON_ATTACK(PacketSessionRef& session, Protocol::C_SATON_ATTACK& pkt);
bool Handle_C_KOUKU_ATTACK(PacketSessionRef& session, Protocol::C_KOUKU_ATTACK& pkt);
bool Handle_C_KOUKU_COUNTER(PacketSessionRef& session, Protocol::C_KOUKU_COUNTER& pkt);
bool Handle_C_DEBUG_PKT(PacketSessionRef& session, Protocol::C_DEBUG_PKT& pkt);
bool Handle_C_KOUKU_DAMAGE(PacketSessionRef& session, Protocol::C_KOUKU_DAMAGE& pkt);
bool Handle_C_KOUKU_RESULT(PacketSessionRef& session, Protocol::C_KOUKU_RESULT& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTER_GAME>(Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_INPUT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_INPUT>(Handle_C_PLAYER_INPUT, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_WORLD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_WORLD>(Handle_C_PLAYER_WORLD, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_YAW_ACTION] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_YAW_ACTION>(Handle_C_PLAYER_YAW_ACTION, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_ACTION] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_ACTION>(Handle_C_PLAYER_ACTION, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_ARROW] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_ARROW>(Handle_C_PLAYER_ARROW, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_EQUIP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_EQUIP>(Handle_C_PLAYER_EQUIP, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_DEAD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_DEAD>(Handle_C_PLAYER_DEAD, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_RESPANW] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_RESPANW>(Handle_C_PLAYER_RESPANW, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_MOVE_STAGE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_MOVE_STAGE>(Handle_C_PLAYER_MOVE_STAGE, session, buffer, len); };
		GPacketHandler[PKT_C_PLAYER_MOVE_STAGE_FINISH] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAYER_MOVE_STAGE_FINISH>(Handle_C_PLAYER_MOVE_STAGE_FINISH, session, buffer, len); };
		GPacketHandler[PKT_C_SPAWN_MONSTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SPAWN_MONSTER>(Handle_C_SPAWN_MONSTER, session, buffer, len); };
		GPacketHandler[PKT_C_MONSTER_SET_TARGET] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MONSTER_SET_TARGET>(Handle_C_MONSTER_SET_TARGET, session, buffer, len); };
		GPacketHandler[PKT_C_MONSTER_WORLD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MONSTER_WORLD>(Handle_C_MONSTER_WORLD, session, buffer, len); };
		GPacketHandler[PKT_C_BOSS_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_BOSS_SPAWN>(Handle_C_BOSS_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_C_BOSS_WORLD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_BOSS_WORLD>(Handle_C_BOSS_WORLD, session, buffer, len); };
		GPacketHandler[PKT_C_SATON_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SATON_ATTACK>(Handle_C_SATON_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_C_KOUKU_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_KOUKU_ATTACK>(Handle_C_KOUKU_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_C_KOUKU_COUNTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_KOUKU_COUNTER>(Handle_C_KOUKU_COUNTER, session, buffer, len); };
		GPacketHandler[PKT_C_DEBUG_PKT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_DEBUG_PKT>(Handle_C_DEBUG_PKT, session, buffer, len); };
		GPacketHandler[PKT_C_KOUKU_DAMAGE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_KOUKU_DAMAGE>(Handle_C_KOUKU_DAMAGE, session, buffer, len); };
		GPacketHandler[PKT_C_KOUKU_RESULT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_KOUKU_RESULT>(Handle_C_KOUKU_RESULT, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_OTHER_PLAYER& pkt) { return MakeSendBuffer(pkt, PKT_S_OTHER_PLAYER); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_INPUT& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_INPUT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_WORLD& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_WORLD); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_YAW_ACTION& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_YAW_ACTION); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_ACTION& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_ACTION); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_ARROW& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_ARROW); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_EQUIP& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_EQUIP); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ALL_PLAYER_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_S_ALL_PLAYER_ENTER); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_DEAD& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_DEAD); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_RESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_RESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_MOVE_STAGE& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_MOVE_STAGE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PLAYER_MOVE_STAGE_FINISH& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAYER_MOVE_STAGE_FINISH); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SPAWN_MONSTER& pkt) { return MakeSendBuffer(pkt, PKT_S_SPAWN_MONSTER); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MONSTER_SET_TARGET& pkt) { return MakeSendBuffer(pkt, PKT_S_MONSTER_SET_TARGET); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MONSTER_WORLD& pkt) { return MakeSendBuffer(pkt, PKT_S_MONSTER_WORLD); }
	static SendBufferRef MakeSendBuffer(Protocol::S_BOSS_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_BOSS_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_BOSS_WORLD& pkt) { return MakeSendBuffer(pkt, PKT_S_BOSS_WORLD); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SATON_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_S_SATON_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::S_KOUKU_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_S_KOUKU_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::S_KOUKU_COUNTER& pkt) { return MakeSendBuffer(pkt, PKT_S_KOUKU_COUNTER); }
	static SendBufferRef MakeSendBuffer(Protocol::S_KOUKU_RESULT& pkt) { return MakeSendBuffer(pkt, PKT_S_KOUKU_RESULT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};