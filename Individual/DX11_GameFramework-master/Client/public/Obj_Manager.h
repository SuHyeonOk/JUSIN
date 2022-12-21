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
		PLAYER		ePlayer = PLAYER_END;		// 현재 플레이어

		enum STATE {
			IDLE, RUN, ROLL, ATTACK, CONTROL, HIT, STUN, DIE, CHANGE, CHEERING, SWIM,
			TREEWITCH, MAGIC, STATE_END
		};
		STATE		eState;		// 현재 상태
		STATE		ePreState;	// 이전 상태

		enum PLAYERWEAPON { F_ROOT, F_DOLDEN, F_FAMILY, SHIELD_END };
		PLAYERWEAPON		ePlayerWeapon = SHIELD_END;

		enum JAKEWEAPON { LFIST, RFIST, SHLDE, JAKEWERPON_END };
		JAKEWEAPON			eJakeWeapon = JAKEWERPON_END;

		enum ITEM { ITEM_END };
		ITEM		eItem = ITEM_END;


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
	void				Set_Current_Player_State(PLAYERINFO::STATE	eState) { m_tPlayerInfo.eState = eState; }

	// 무기
	void				Set_Jake_Shield() { m_bShield = true; }
	void				Set_Jake_Weapon(PLAYERINFO::JAKEWEAPON	eWeapon) { m_tPlayerInfo.eJakeWeapon = eWeapon; }

	// 플레이어의 공격력 몬스터에게 주기 위해서
	_int				Get_Player_Attack() { return m_tPlayerInfo.iAttack; }

	// UI 관련
	void				Set_Player_Exp(_int eExp) { m_tPlayerInfo.iExp += eExp; }
	void				Set_Player_PushHp(_int eHp) { if (m_tPlayerInfo.iHp <= m_tPlayerInfo.iHpMax) m_tPlayerInfo.iHp += eHp; }
	void				CObj_Manager::Set_Player_MinusHp(_int eHp);

public:	// 다른 객체에세 플레이어의 주소를 전달하기 위한 기능, 거리 계산 기능
	void				CObj_Manager::Tick_Player_Transform();					// Player 에서 현재 내 거리를 계산한다.
	_vector				CObj_Manager::Get_Player_Transform();					// 현재 Player 의 POS 를 가져온다.
	_float				CObj_Manager::Get_Player_Distance(_fvector	_MyPos);	// Player 와 내 거리

	// 플레이어 스킬에서 Rotation() 을 위해서
	_float				Get_Angle() { return XMConvertToRadians(m_fAngle); }
	void				Set_Angle(_float fAngle) { m_fAngle = fAngle; }

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
	_bool			m_bShield = false;

	_float			m_fAngle = 0.f;

private:
	// X 키를 누를 때 마다 Player 가 달라진다. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;

	_int			m_iMonster_Attck = 0;
	_double			m_dPlayerAttck_TimeAcc = 0;

public:
	virtual void Free() override;
};

END