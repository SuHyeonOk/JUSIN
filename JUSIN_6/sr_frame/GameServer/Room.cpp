#include "stdafx.h"
#include "Room.h"

#include "ClientPacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "Protocol.pb.h"

shared_ptr<Room> GRoom = make_shared<Room>();

Room::Room()
{
	_playerNum = 2;
}

void Room::Enter(PlayerRef player)
{
	_players[player->playerId] = player;

	if (_players.empty() == false)
	{
		Protocol::S_OTHER_PLAYER otherPkt;
		for (auto& e : _players)
		{
			auto repeatedPlayer = otherPkt.add_player();
			repeatedPlayer->set_id(e.second->playerId);
			repeatedPlayer->set_name(e.second->name);
			otherPkt.add_playerskin(e.second->type);
		}

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(otherPkt);
		DoAsync(&Room::Broadcast, sendBuffer);
	}

	// if (_players.size() == _playerNum)
	// {
	// 	Protocol::S_ALL_PLAYER_ENTER allPkt;
	// 	allPkt.set_success(true);
	// 	DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(allPkt));
	// }
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		if (const auto locked = p.second->ownerSession.lock())
		{
			locked->Send(sendBuffer);
		}
	}
}

void Room::Dead(uint64 iID)
{
	_players.find(iID)->second->m_bDead = true;

	uint64 iDeadCnt = 0;
	for (auto& p : _players)
	{
		if (p.second->m_bDead)
			++iDeadCnt;
	}

	Protocol::S_PLAYER_DEAD deadPkt;
	deadPkt.set_success(true);
	deadPkt.mutable_player()->set_id(iID);

	DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(deadPkt));


	if (iDeadCnt == _playerNum)
	{
		for (auto& p : _players)
			p.second->m_bDead = false;

		Protocol::S_PLAYER_RESPAWN respawnPkt;
		respawnPkt.set_success(true);

		DoTimer(3000, &Room::Broadcast, ClientPacketHandler::MakeSendBuffer(respawnPkt));
	}
}

void Room::DoDamage(uint32 iID, uint32 damage)
{
	_players.find(iID)->second->totalDamage += damage;
}

void Room::DoCounter(uint32 iID)
{
	if (bCounterLock) return;

	bCounterLock.store(true);
	_players.find(iID)->second->counterCnt++;
}

void Room::BroadcastResult()
{
	vector<PlayerRef> players;
	for (auto player : _players)
		players.push_back(player.second);

	std::sort(players.begin(), players.end(), [](PlayerRef p1, PlayerRef p2)
	{
		return p1->totalDamage > p2->totalDamage;
	});

	Protocol::S_KOUKU_RESULT resultPkt;
	resultPkt.set_success(true);
	for (auto player : _players)
	{
		cout << "player " << player.second->name << " : " << player.second->totalDamage<<endl;
		auto repeatedResult = resultPkt.add_result();
		repeatedResult->mutable_player()->set_id(player.first);
		repeatedResult->mutable_player()->set_name(player.second->name);
		repeatedResult->set_skin(player.second->type);
		repeatedResult->set_damage(player.second->totalDamage);
		repeatedResult->set_counter(player.second->counterCnt);
	}

	DoAsync(&Room::Broadcast, ClientPacketHandler::MakeSendBuffer(resultPkt));
}
