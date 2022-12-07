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

		PLAYER	ePlayer = PLAYER_END;		// ���� �÷��̾�

		STATE	eState;		// ���� ����
		STATE	ePreState;	// ���� ����
		ITEM	eItem;

		_int	iHp;		// ����"��" ü���� ���� �� �ʿ�
		_int	iHpMax;
		_int	iAttack;	// ����"��" ü���� ���� �� �ʿ�
		_int	iExp;		// ���Ͱ� ������ ����
		_int	iExpMax;
		_int	iLevel;		// iExp�� �������� +1 ����
		_int	iCoin;		// Coin �� �浹�ϸ� ����

		_int	iKey;		// Key �� �浹�ϸ� ����
		_int	iHeart;

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	// ���� Level
	LEVEL				Get_Current_Level() { return m_eCurrent_Level; }
	void				Set_Current_Level(LEVEL eLevelID) { m_eCurrent_Level = eLevelID; }

	// Player ����
	PLAYERINFO			Get_Current_Player() { return m_tPlayerInfo; }
	void				Set_Current_Player(PLAYERINFO ePlayerInfo) { m_tPlayerInfo = ePlayerInfo; }
	PLAYERINFO::STATE	Get_Current_Player_State() { return m_tPlayerInfo.eState; }
	void				Set_Current_Player_State(PLAYERINFO::STATE	eState) { m_tPlayerInfo.eState = eState; }

	// �÷��̾��� ���ݷ� ���Ϳ��� �ֱ� ���ؼ�
	_int				Get_Player_Attack() { return m_tPlayerInfo.iAttack; }	

	// UI ����
	void				Set_Player_Exp(_int eExp) { m_tPlayerInfo.iExp += eExp; }
	void				Set_Player_PushHp(_int eHp) { if (m_tPlayerInfo.iHp <= m_tPlayerInfo.iHpMax) m_tPlayerInfo.iHp += eHp; }
	void				Set_Player_MinusHp(_int eHp) { if (0 < m_tPlayerInfo.iHp) m_tPlayerInfo.iHp -= eHp; }

	// ��ǥ
	void				CObj_Manager::Tick_Player_Transform(); 
	_vector				CObj_Manager::Get_Player_Transform(); 

public:
	HRESULT		CObj_Manager::Initialized();
	void		CObj_Manager::Tick(_double TimeDelta);

private:
	void		CObj_Manager::Current_Player();	// ���� �÷��̾ �������� Tick
	void		CObj_Manager::Player_Exp();		// ���� �÷��̾��� ����ġ�� Tick

private:
	LEVEL			m_eCurrent_Level;
	PLAYERINFO		m_tPlayerInfo;

private:
	// X Ű�� ���� �� ���� Player �� �޶�����. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;

public:
	virtual void Free() override;
};

END