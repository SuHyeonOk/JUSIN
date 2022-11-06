#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	Room();
	// 싱글쓰레드 환경인마냥 코딩
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	void Dead(uint64 iID);
	void DoDamage(uint32 iID, uint32 damage);
	void DoCounter(uint32 iID);
	void BroadcastResult();
	void CounterUnlock(){bCounterLock.store(false);}

	map<uint64, PlayerRef> _players;

	uint64 _playerNum;

	Atomic<bool> bCounterLock{false};
};

extern shared_ptr<Room> GRoom;