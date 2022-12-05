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
		enum STATE { IDLE, ATTACK, HIT, DIE, STATE_END };
		enum ITEM { ITEM_END };

		PLAYER	ePlayer = PLAYER_END;		// ���� �÷��̾�
		LEVEL	ePlayer_Level = LEVEL_END;	// ���� �÷��̾ �ִ� Level

		STATE	eState;	// �̰� �÷��̾� ����?
		ITEM	eItem;

		_uint	iHp;
		_uint	iAttack;
		_uint	iExp;
		_uint	iExpMax;
		_uint	iLevel;
		_uint	iGold;

		_uint	iKey;
		_uint	iHeart;

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	PLAYERINFO			Get_Current_Player() { return m_ePlayerInfo; }

	void				Set_Current_Player(PLAYERINFO ePlayerInfo) { m_ePlayerInfo = ePlayerInfo; }
	void				Set_Plyaer_Level(LEVEL eLevelID) { m_ePlayerInfo.ePlayer_Level = eLevelID; }

	_vector				CObj_Manager::Get_Player_Transform(); // TODO

public:
	HRESULT		CObj_Manager::Initialized();
	void		CObj_Manager::Tick(_double TimeDelta);

private:
	void		Current_Player();	// ���� �÷��̾ �������� Tick �� ���� �Լ�

private:
	PLAYERINFO		m_ePlayerInfo;

private:
	// X Ű�� ���� �� ���� Player �� �޶�����. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;

public:
	virtual void Free() override;	
};

END