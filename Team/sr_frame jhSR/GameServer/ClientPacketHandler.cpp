#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"
#include "Service.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

enum PlayerInputMask
{
	PLAYER_W = 1,
	PLAYER_A = 1 << 1,
	PLAYER_S = 1 << 2,
	PLAYER_D = 1 << 3,

	PLAYER_ML = 1 << 4,
	PLAYER_MR = 1 << 5,
	PLAYER_1 = 1 << 6,
	PLAYER_2 = 1 << 7,
	PLAYER_3 = 1 << 8,
	PLAYER_ROLL = 1 << 9,
	PLAYER_POTION = 1 << 10,
};


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	// TODO : Validation 체크

	Protocol::C_PLAYER_INPUT t;

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	// DB에서 플레이 정보를 긁어온다
	// GameSession에 플레이 정보를 저장 (메모리)

	// ID 발급 (DB 아이디가 아니고, 인게임 아이디)
	static Atomic<uint64> idGenerator{0};

	{
		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		cout << "Player : " << playerRef->playerId << " login "<<endl;
		playerRef->ownerSession = gameSession;
		
		gameSession->_currentPlayer = playerRef;

		loginPkt.set_playerid(playerRef->playerId);
		loginPkt.set_success(true);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);
	
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	Protocol::Player playerPkt = pkt.player();

	// uint64 index = playerPkt.id();
	gameSession->_currentPlayer->name = playerPkt.name();
	gameSession->_currentPlayer->type = pkt.playerskin();
	gameSession->_currentPlayer->playerId = playerPkt.id();
	std::cout << "Enter Player :  " << playerPkt.id() << " " << playerPkt.name() << endl;

	Protocol::S_ENTER_GAME enterPkt;
	enterPkt.set_success(true);
	enterPkt.mutable_player()->set_id(playerPkt.id());
	enterPkt.mutable_player()->set_name(playerPkt.name());
	enterPkt.set_playerskin(pkt.playerskin());


	GRoom->DoAsync(&Room::Enter, gameSession->_currentPlayer);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterPkt);
	session->Send(sendBuffer);


	// send mon spawn
	// Protocol::S_SPAWN_MONSTER monPkt;
	// monPkt.set_success(true);
	// monPkt.set_id(1);
	// monPkt.set_factory("Zombie");
	// session->Send(ClientPacketHandler::MakeSendBuffer(monPkt));

	return true;
}

bool Handle_C_PLAYER_INPUT(PacketSessionRef& session, Protocol::C_PLAYER_INPUT& pkt)
{
	Protocol::S_PLAYER_INPUT inputPkt;
	inputPkt.set_success(true);
	inputPkt.mutable_player()->set_id(pkt.player().id());
	inputPkt.mutable_player()->set_name(pkt.player().name());
	inputPkt.set_inputbit(pkt.inputbit());

	std::cout << "Player move :  " << pkt.player().id() << endl;


	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(inputPkt));

	return true;
}

bool Handle_C_PLAYER_WORLD(PacketSessionRef& session, Protocol::C_PLAYER_WORLD& pkt)
{
	Protocol::S_PLAYER_WORLD worldPkt;
	worldPkt.set_success(true);
	worldPkt.mutable_player()->CopyFrom(pkt.player());
	worldPkt.mutable_matworld()->CopyFrom(pkt.matworld());

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(worldPkt));

	return true;
}

bool Handle_C_PLAYER_YAW_ACTION(PacketSessionRef& session, Protocol::C_PLAYER_YAW_ACTION& pkt)
{
	Protocol::S_PLAYER_YAW_ACTION yawActionPkt;
	yawActionPkt.set_success(true);
	yawActionPkt.mutable_player()->CopyFrom(pkt.player());
	yawActionPkt.set_yaw(pkt.yaw());
	yawActionPkt.set_actionbit(pkt.actionbit());
	if (pkt.actionbit() & PLAYER_ROLL)
		std::cout << "Player roll :  " << pkt.player().id() << endl;
	else if (pkt.actionbit() & PLAYER_ML)
		std::cout << "Player melee :  " << pkt.player().id() << endl;

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(yawActionPkt));
	return true;
}

bool Handle_C_PLAYER_ACTION(PacketSessionRef& session, Protocol::C_PLAYER_ACTION& pkt)
{
	Protocol::S_PLAYER_ACTION ActionPkt;
	ActionPkt.set_success(true);
	ActionPkt.mutable_player()->CopyFrom(pkt.player());
	ActionPkt.set_actionbit(pkt.actionbit());
	if (pkt.actionbit() & PLAYER_POTION)
	{
		std::cout << "Player POTION :  " << pkt.player().id() << endl;
	}


	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(ActionPkt));
	return true;
}

bool Handle_C_PLAYER_ARROW(PacketSessionRef& session, Protocol::C_PLAYER_ARROW& pkt)
{
	Protocol::S_PLAYER_ARROW arrowPkt;
	arrowPkt.set_success(true);
	arrowPkt.mutable_player()->CopyFrom(pkt.player());
	arrowPkt.set_yaw(pkt.yaw());
	arrowPkt.mutable_vlookat()->CopyFrom(pkt.vlookat());
	arrowPkt.set_actionbit(pkt.actionbit());

	if (pkt.actionbit() & PLAYER_MR)
	{
		std::cout << "Player arrow :  " << pkt.player().id() << endl;
	}

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(arrowPkt));

	return true;
}

bool Handle_C_PLAYER_EQUIP(PacketSessionRef& session, Protocol::C_PLAYER_EQUIP& pkt)
{
	Protocol::S_PLAYER_EQUIP equipPkt;
	equipPkt.set_success(true);
	equipPkt.mutable_player()->CopyFrom(pkt.player());
	equipPkt.mutable_state()->CopyFrom(pkt.state());

	std::cout << "Player Equip Change :  " << pkt.player().id() << endl;

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(equipPkt));

	return true;
}

bool Handle_C_PLAYER_DEAD(PacketSessionRef& session, Protocol::C_PLAYER_DEAD& pkt)
{
	Protocol::S_PLAYER_DEAD deadPkt;
	deadPkt.set_success(true);
	deadPkt.mutable_player()->CopyFrom(pkt.player());

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(deadPkt));
	return true;
}

bool Handle_C_PLAYER_RESPANW(PacketSessionRef& session, Protocol::C_PLAYER_RESPANW& pkt)
{
	Protocol::S_PLAYER_RESPAWN respawnPkt;
	respawnPkt.set_success(true);
	respawnPkt.mutable_player()->CopyFrom(pkt.player());
	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(respawnPkt));

	return true;
}

bool Handle_C_PLAYER_MOVE_STAGE(PacketSessionRef& session, Protocol::C_PLAYER_MOVE_STAGE& pkt)
{
	Protocol::S_PLAYER_MOVE_STAGE movePkt;
	movePkt.set_success(true);
	movePkt.set_loadingtag(pkt.loadingtag());
	movePkt.set_stagetag(pkt.stagetag());
	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(movePkt));
	return true;
}

bool Handle_C_PLAYER_MOVE_STAGE_FINISH(PacketSessionRef& session, Protocol::C_PLAYER_MOVE_STAGE_FINISH& pkt)
{
	Protocol::S_PLAYER_MOVE_STAGE_FINISH finPkt;
	finPkt.mutable_player()->CopyFrom(pkt.player());
	finPkt.set_playerskin(pkt.playerskin());
	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(finPkt));
	return true;
}

bool Handle_C_SPAWN_MONSTER(PacketSessionRef& session, Protocol::C_SPAWN_MONSTER& pkt)
{
	// Protocol::S_SPAWN_MONSTER monPkt;
	// monPkt.set_success(true);
	// monPkt.set_id(pkt.id());
	// monPkt.set_factory(pkt.factory());
	// monPkt.mutable_matrix()->CopyFrom(pkt.matrix());
	//
	// GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(monPkt));

	return true;
}

bool Handle_C_MONSTER_SET_TARGET(PacketSessionRef& session, Protocol::C_MONSTER_SET_TARGET& pkt)
{
	Protocol::S_MONSTER_SET_TARGET monPkt;
	monPkt.set_success(true);
	monPkt.set_monsterid(pkt.monsterid());
	monPkt.set_playerid(pkt.playerid());
	monPkt.set_battack(pkt.battack());
	if (pkt.battack())
	{
		cout << "Monster " << pkt.monsterid() << " attack to " << pkt.playerid() <<endl;
	}
	else
	{
		cout << "Monster " << pkt.monsterid() << " move to " << pkt.playerid() <<endl;
	}

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(monPkt));
	return true;
}

bool Handle_C_MONSTER_WORLD(PacketSessionRef& session, Protocol::C_MONSTER_WORLD& pkt)
{
	Protocol::S_MONSTER_WORLD monPkt;
	monPkt.set_success(true);
	monPkt.set_monsterid(pkt.monsterid());
	monPkt.mutable_matworld()->CopyFrom(pkt.matworld());

	cout << "Monster " << pkt.monsterid() << " world renew" << endl;

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(monPkt));

	return true;
}

bool Handle_C_BOSS_SPAWN(PacketSessionRef& session, Protocol::C_BOSS_SPAWN& pkt)
{
	Protocol::S_BOSS_SPAWN bossSpawnPkt;
	bossSpawnPkt.set_success(true);
	bossSpawnPkt.set_factory(pkt.factory());
	bossSpawnPkt.mutable_matrix()->CopyFrom(pkt.matrix());

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(bossSpawnPkt));

	return true;
}

bool Handle_C_BOSS_WORLD(PacketSessionRef& session, Protocol::C_BOSS_WORLD& pkt)
{
	Protocol::S_BOSS_WORLD worldPkt;
	worldPkt.set_success(true);
	worldPkt.set_objkey(pkt.objkey());
	worldPkt.mutable_matworld()->CopyFrom(pkt.matworld());
	worldPkt.set_ihp(pkt.ihp());

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(worldPkt));

	return true;
}

bool Handle_C_SATON_ATTACK(PacketSessionRef& session, Protocol::C_SATON_ATTACK& pkt)
{
	Protocol::S_SATON_ATTACK satonPkt;
	satonPkt.set_success(true);
	satonPkt.set_pattern(pkt.pattern());
	satonPkt.mutable_targetpos()->CopyFrom(pkt.targetpos());

	float x = pkt.targetpos().x();
	float y = pkt.targetpos().y();
	float z = pkt.targetpos().z();

	switch (pkt.pattern())
	{
	case Protocol::MoveTo:
		// cout << "Saton Moveto " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::HAMMER:
		cout << "Saton HAMMER " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::GRAB:
		cout << "Saton GRAB " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::BIRD:
		cout << "Saton BIRD " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::SYMBOL:
		cout << "Saton SYMBOL " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::FASCINATE:
		cout << "Saton FASCINATE " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::DRAWMOON:
		cout << "Saton DRAWMOON " << x << " " << y << " " << z <<endl;
		break;
	case Protocol::SatonPattern_INT_MIN_SENTINEL_DO_NOT_USE_: break;
	case Protocol::SatonPattern_INT_MAX_SENTINEL_DO_NOT_USE_: break;
	default: ;
	}

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(satonPkt));

	return true;
}

bool Handle_C_KOUKU_ATTACK(PacketSessionRef& session, Protocol::C_KOUKU_ATTACK& pkt)
{
	Protocol::S_KOUKU_ATTACK koukuPkt;
	koukuPkt.set_success(true);
	koukuPkt.set_pattern(pkt.pattern());
	koukuPkt.mutable_targetpos()->CopyFrom(pkt.targetpos());

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(koukuPkt));

	return true;
}

bool Handle_C_KOUKU_COUNTER(PacketSessionRef& session, Protocol::C_KOUKU_COUNTER& pkt)
{
	Protocol::S_KOUKU_COUNTER counterPkt;
	counterPkt.set_success(true);

	GRoom->DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(counterPkt));
	GRoom->DoAsync(&Room::DoCounter, (uint32)pkt.player().id());

	GRoom->DoTimer(3000, &Room::CounterUnlock);
	return true;
}

bool Handle_C_DEBUG_PKT(PacketSessionRef& session, Protocol::C_DEBUG_PKT& pkt)
{
	cout << pkt.debuglog() <<endl;
	return true;
}

bool Handle_C_KOUKU_DAMAGE(PacketSessionRef& session, Protocol::C_KOUKU_DAMAGE& pkt)
{
	GRoom->DoAsync(&Room::DoDamage, (uint32)pkt.player().id(), pkt.damage());
	return true;
}

bool Handle_C_KOUKU_RESULT(PacketSessionRef& session, Protocol::C_KOUKU_RESULT& pkt)
{
	GRoom->DoTimer(5000, &Room::BroadcastResult);
	return true;
}

