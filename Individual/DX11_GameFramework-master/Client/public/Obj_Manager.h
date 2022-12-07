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
		enum STATE { IDLE, RUN, ATTACK, HIT, STUN, DIE, STATE_END };
		enum ITEM { ITEM_END };

		PLAYER	ePlayer = PLAYER_END;		// 현재 플레이어

		STATE	eState;		// 현재 상태
		STATE	ePreState;	// 이전 상태
		ITEM	eItem;

		_int	iHp;		// 몬스터"가" 체력을 깍을 때 필요
		_int	iHpMax;
		_int	iAttack;	// 몬스터"의" 체력을 깍을 때 필요
		_int	iExp;		// 몬스터가 죽으면 증가
		_int	iExpMax;
		_int	iLevel;		// iExp가 가득차면 +1 증가
		_int	iCoin;		// Coin 과 충돌하면 증가

		_int	iKey;		// Key 와 충돌하면 증가
		_int	iHeart;

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	// 현재 Level
	LEVEL				Get_Current_Level() { return m_eCurrent_Level; }
	void				Set_Current_Level(LEVEL eLevelID) { m_eCurrent_Level = eLevelID; }

	// Player 정보
	PLAYERINFO			Get_Current_Player() { return m_tPlayerInfo; }
	void				Set_Current_Player(PLAYERINFO ePlayerInfo) { m_tPlayerInfo = ePlayerInfo; }
	PLAYERINFO::STATE	Get_Current_Player_State() { return m_tPlayerInfo.eState; }
	void				Set_Current_Player_State(PLAYERINFO::STATE	eState) { m_tPlayerInfo.eState = eState; }

	// 플레이어의 공격력 몬스터에게 주기 위해서
	_int				Get_Player_Attack() { return m_tPlayerInfo.iAttack; }	

	// UI 관련
	void				Set_Player_Exp(_int eExp) { m_tPlayerInfo.iExp += eExp; }
	void				Set_Player_PushHp(_int eHp) { if (m_tPlayerInfo.iHp <= m_tPlayerInfo.iHpMax) m_tPlayerInfo.iHp += eHp; }
	void				Set_Player_MinusHp(_int eHp) { if (0 < m_tPlayerInfo.iHp) m_tPlayerInfo.iHp -= eHp; }

	// 좌표
	void				CObj_Manager::Tick_Player_Transform(); 
	_vector				CObj_Manager::Get_Player_Transform(); 

public:
	HRESULT		CObj_Manager::Initialized();
	void		CObj_Manager::Tick(_double TimeDelta);

private:
	void		CObj_Manager::Current_Player();	// 현재 플레이어가 누구인지 Tick
	void		CObj_Manager::Player_Exp();		// 현재 플레이어의 경험치를 Tick

private:
	LEVEL			m_eCurrent_Level;
	PLAYERINFO		m_tPlayerInfo;

private:
	// X 키를 누를 때 마다 Player 가 달라진다. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;

public:
	virtual void Free() override;
};

END