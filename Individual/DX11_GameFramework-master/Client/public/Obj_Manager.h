#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CObj_Manager : public CBase
{
	DECLARE_SINGLETON(CObj_Manager)

public:
	_bool	Get_NavigationRender() { return g_bNavigationRender; }

public:
	typedef struct tagPlayerInfo
	{
		enum PLAYER { FINN, JAKE, FREE, RESET, PLAYER_END };
		PLAYER		ePlayer = PLAYER_END;		// ���� �÷��̾�

		enum STATE {
			IDLE, RUN, ROLL, ATTACK, CONTROL, HIT, KNOCKBACKHIT, STUN, DIE, CHANGE, CHEERING, LEVEL_UP,
			SWIM, S_PAINT, S_MARCELINE, S_COIN, S_FIONA,
			TREEWITCH_0, TREEWITCH_1, TREEWITCH_2, MAGIC, STATE_END
		};
		STATE		eState = STATE_END;		// ���� ����
		STATE		ePreState = STATE_END;	// ���� ����

		enum PLAYERWEAPON { F_DOLDEN, F_ROOT, F_FAMILY, SHIELD_END };
		PLAYERWEAPON		ePlayerWeapon = SHIELD_END;

		enum JAKEWEAPON { LFIST, RFIST, SHIELD, JAKEWERPON_END };
		JAKEWEAPON			eJakeWeapon = JAKEWERPON_END;

		enum ITEM { ITEM_END };
		ITEM		eItem = ITEM_END;


		_float	fHP;		// ����"��" ü���� ���� �� �ʿ�
		_float	fHPMax;
		_float	fAttack;	// ����"��" ü���� ���� �� �ʿ�
		_float	fExp;		// ���Ͱ� ������ ����
		_float	fExpMax;
		_int	iLevel;		// iExp�� �������� +1 ����
		_int	iCoin;		// Coin �� �浹�ϸ� ����

		_int	iKey;		// Key �� �浹�ϸ� ����
		_int	iHeart;

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	_bool				Get_Monster_Crash() { return m_bMonster_Crash; }
	void				Set_Monster_Crash(_bool bCrash) { m_bMonster_Crash = bCrash; }

	// ���� ������ �Ѿ �� ����.
	_bool				Get_NextLevel() { return m_bNextLevel; }
	void				Set_NextLevel(_bool bNext) { m_bNextLevel = bNext; }

	// ���� Level
	LEVEL				Get_Current_Level() { return m_eCurrent_Level; }
	void				Set_Current_Level(LEVEL eLevelID) { m_eCurrent_Level = eLevelID; }

	// Player ����
	PLAYERINFO			Get_Current_Player() { return m_tPlayerInfo; }
	void				Set_Current_Player(PLAYERINFO ePlayerInfo) { m_tPlayerInfo = ePlayerInfo; }
	void				Set_Current_Player_State(PLAYERINFO::STATE	eState) { m_tPlayerInfo.eState = eState; }

	// ����
	_bool				Get_Jake_Shield() { return m_bShield; }
	void				Set_Jake_Shield(_bool bShield) { m_bShield = bShield; }
	void				Set_Jake_Weapon(PLAYERINFO::JAKEWEAPON	eWeapon) { m_tPlayerInfo.eJakeWeapon = eWeapon; }

	// �÷��̾��� ���ݷ� ���Ϳ��� �ֱ� ���ؼ�
	_float				Get_Player_Attack() { return m_tPlayerInfo.fAttack; }
	void				Set_Player_Attack(_float iAttack) { m_tPlayerInfo.fAttack = iAttack; } // ��ų�� ��� ��� ���ݷ��� �÷ȴٰ� �������� �ϱ� ���ؼ�

	// UI ����
	void				CObj_Manager::Set_Player_PlusExp(_float fExp) { m_tPlayerInfo.fExp += fExp; }	// TODO : ������ �� ���� �Լ� ���� �ؾ��ϴµ� �̹� ���Ϳ��� ���� �� ó����..����
	void				CObj_Manager::Set_Player_PlusHP(_float fHP);
	void				CObj_Manager::Set_Player_MinusHP(_float fAttack);

	void				Set_Coin(_int iCount) { m_tPlayerInfo.iCoin += iCount; }
	void				Set_Key() { m_tPlayerInfo.iKey += 1; }
	void				Set_Heart() { m_tPlayerInfo.iHeart += 1; }

	// UI Talk
	_bool				Get_Interaction() { return m_bInteraction; }

public:	// �ٸ� ��ü���� �÷��̾��� �ּҸ� �����ϱ� ���� ���, �Ÿ� ��� ���
	void				CObj_Manager::Tick_Player_Transform();					// Player ���� ���� �� �Ÿ��� ����Ѵ�.
	_vector				CObj_Manager::Get_Player_Transform();					// ���� Player �� POS �� �����´�.
	_float				CObj_Manager::Get_Player_Distance(_fvector	_MyPos);	// Player �� �� �Ÿ�

	// �÷��̾� ��ų���� Rotation() �� ���ؼ�
	_float				Get_Angle() { return XMConvertToRadians(m_fAngle); }
	void				Set_Angle(_float fAngle) { m_fAngle = fAngle; }

public:
	HRESULT		CObj_Manager::Initialized();
	void		CObj_Manager::Tick(_double TimeDelta);

private:
	void		CObj_Manager::Current_Player();		// ���� �÷��̾ �������� Tick
	void		CObj_Manager::Player_Exp(const _double & TimeDelta);			// ���� �÷��̾��� ����ġ�� Tick
	void		CObj_Manager::Player_Weapon();		// ���� �÷��̾��� ���� ���

private:
	void		CObj_Manager::Key_Input();

private:
	_bool			m_bNextLevel = true;	// ó���� true ���ٰ� ������ �����ϸ� flase �� �ʱ�ȭ �Ѵ�. �׸���, ���� ���� �Ѿ �� true �� �ȴ�.
	LEVEL			m_eCurrent_Level;
	PLAYERINFO		m_tPlayerInfo;

private: // KeyInput
	_bool			g_bNavigationRender = false;
	_bool			m_bInteraction = false;

private:
	_bool			m_bShield = false;
	_bool			m_bMonster_Crash = false;	// �÷��̾ ���� ���Ϳ� �浹 �ߴ���

	_float			m_fAngle = 0.f;

private:
	// X Ű�� ���� �� ���� Player �� �޶�����. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;

	_float			m_fMonster_Attck = 0;
	_double			m_dPlayerAttck_TimeAcc = 0;

	_int			m_iWeapon_Index = 0;

	_bool			m_bEffect = false;
	_double			m_dEffect_TimeAcc = 0;
	_double			m_dEffect_Up_TimeAcc = 0;

public:
	virtual void Free() override;
};

END