#pragma once

class Player
{
public:

	uint64					playerId = 0;
	string					name;
	Protocol::PlayerSkin	type = Protocol::PLAYER_TYPE_STEVE;
	weak_ptr<GameSession>			ownerSession; // Cycle
	array<float, 3> vPos;
	bool m_bDead = false;

	uint32 totalDamage = 0;
	uint32 counterCnt = 0;
};

