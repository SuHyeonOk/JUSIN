#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CObj_Manager : public CBase
{
	DECLARE_SINGLETON(CObj_Manager)

public:
	typedef struct tagPlayerInfo
	{
		enum PLAYER { FINN, JAKE, FREE, RESET, PLAYER_END };

		PLAYER	ePlayer = PLAYER_END;		// 현재 플레이어
		LEVEL	ePlayer_Level = LEVEL_END;	// 현재 플레이어가 있는 Level

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	PLAYERINFO	Get_Current_Player() { return m_ePlayerInfo; }
	_vector		Get_Player_Transform();

	void		Set_Plyaer_Level(LEVEL eLevelID) { m_ePlayerInfo.ePlayer_Level = eLevelID; }

public:
	void		Tick(_double TimeDelta);

private:
	void		Current_Player();	// 현재 플레이어가 누구인지 Tick 을 도는 함수

private:
	// X 키를 누를 때 마다 Player 가 달라진다. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;
	PLAYERINFO		m_ePlayerInfo;

public:
	virtual void Free() override;	
};

END