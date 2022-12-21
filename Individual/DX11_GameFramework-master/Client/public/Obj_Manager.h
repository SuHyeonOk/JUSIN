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
	typedef struct tagPlayerInfo
	{
		enum PLAYER { FINN, JAKE, FREE, RESET, PLAYER_END };
		PLAYER		ePlayer = PLAYER_END;		// ���� �÷��̾�

		enum STATE {
			IDLE, RUN, ROLL, ATTACK, CONTROL, HIT, STUN, DIE, CHANGE, CHEERING, SWIM,
			TREEWITCH, MAGIC, STATE_END
		};
		STATE		eState;		// ���� ����
		STATE		ePreState;	// ���� ����

		enum PLAYERWEAPON { F_ROOT, F_DOLDEN, F_FAMILY, SHIELD_END };
		PLAYERWEAPON		ePlayerWeapon = SHIELD_END;

		enum JAKEWEAPON { LFIST, RFIST, SHLDE, JAKEWERPON_END };
		JAKEWEAPON			eJakeWeapon = JAKEWERPON_END;

		enum ITEM { ITEM_END };
		ITEM		eItem = ITEM_END;


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
	void				Set_Current_Player_State(PLAYERINFO::STATE	eState) { m_tPlayerInfo.eState = eState; }

	// ����
	void				Set_Jake_Shield() { m_bShield = true; }
	void				Set_Jake_Weapon(PLAYERINFO::JAKEWEAPON	eWeapon) { m_tPlayerInfo.eJakeWeapon = eWeapon; }

	// �÷��̾��� ���ݷ� ���Ϳ��� �ֱ� ���ؼ�
	_int				Get_Player_Attack() { return m_tPlayerInfo.iAttack; }

	// UI ����
	void				Set_Player_Exp(_int eExp) { m_tPlayerInfo.iExp += eExp; }
	void				Set_Player_PushHp(_int eHp) { if (m_tPlayerInfo.iHp <= m_tPlayerInfo.iHpMax) m_tPlayerInfo.iHp += eHp; }
	void				CObj_Manager::Set_Player_MinusHp(_int eHp);

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
	void		CObj_Manager::Current_Player();	// ���� �÷��̾ �������� Tick
	void		CObj_Manager::Player_Exp();		// ���� �÷��̾��� ����ġ�� Tick

private:
	LEVEL			m_eCurrent_Level;
	PLAYERINFO		m_tPlayerInfo;

private:
	_bool			m_bShield = false;

	_float			m_fAngle = 0.f;

private:
	// X Ű�� ���� �� ���� Player �� �޶�����. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;

	_int			m_iMonster_Attck = 0;
	_double			m_dPlayerAttck_TimeAcc = 0;

public:
	virtual void Free() override;
};

END