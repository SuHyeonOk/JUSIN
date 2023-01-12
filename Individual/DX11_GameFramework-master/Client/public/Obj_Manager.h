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
		PLAYER		ePlayer = PLAYER_END;		// 현재 플레이어

		enum STATE {
			IDLE, RUN, ROLL, ATTACK, CONTROL, HIT, KNOCKBACKHIT, STUN, DIE, CHANGE, CHEERING, LEVEL_UP,
			SWIM, S_PAINT, S_MARCELINE, S_COIN, S_FIONA,
			TREEWITCH_0, TREEWITCH_1, TREEWITCH_2, MAGIC, STATE_END
		};
		STATE		eState = STATE_END;		// 현재 상태
		STATE		ePreState = STATE_END;	// 이전 상태

		enum PLAYERWEAPON { F_DOLDEN, F_ROOT, F_FAMILY, SHIELD_END };
		PLAYERWEAPON		ePlayerWeapon = SHIELD_END;

		enum JAKEWEAPON { LFIST, RFIST, SHIELD, JAKEWERPON_END };
		JAKEWEAPON			eJakeWeapon = JAKEWERPON_END;

		enum ITEM { ITEM_END };
		ITEM		eItem = ITEM_END;


		_float	fHP;		// 몬스터"가" 체력을 깍을 때 필요
		_float	fHPMax;
		_float	fAttack;	// 몬스터"의" 체력을 깍을 때 필요
		_float	fExp;		// 몬스터가 죽으면 증가
		_float	fExpMax;
		_int	iLevel;		// iExp가 가득차면 +1 증가
		_int	iCoin;		// Coin 과 충돌하면 증가

		_int	iKey;		// Key 와 충돌하면 증가
		_int	iHeart;

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	_bool				Get_Monster_Crash() { return m_bMonster_Crash; }
	void				Set_Monster_Crash(_bool bCrash) { m_bMonster_Crash = bCrash; }

	// 다음 레벨로 넘어갈 것 인지.
	_bool				Get_NextLevel() { return m_bNextLevel; }
	void				Set_NextLevel(_bool bNext) { m_bNextLevel = bNext; }

	// 현재 Level
	LEVEL				Get_Current_Level() { return m_eCurrent_Level; }
	void				Set_Current_Level(LEVEL eLevelID) { m_eCurrent_Level = eLevelID; }

	// Player 정보
	PLAYERINFO			Get_Current_Player() { return m_tPlayerInfo; }
	void				Set_Current_Player(PLAYERINFO ePlayerInfo) { m_tPlayerInfo = ePlayerInfo; }
	void				Set_Current_Player_State(PLAYERINFO::STATE	eState) { m_tPlayerInfo.eState = eState; }

	// 무기
	_bool				Get_Jake_Shield() { return m_bShield; }
	void				Set_Jake_Shield(_bool bShield) { m_bShield = bShield; }
	void				Set_Jake_Weapon(PLAYERINFO::JAKEWEAPON	eWeapon) { m_tPlayerInfo.eJakeWeapon = eWeapon; }

	// 플레이어의 공격력 몬스터에게 주기 위해서
	_float				Get_Player_Attack() { return m_tPlayerInfo.fAttack; }
	void				Set_Player_Attack(_float iAttack) { m_tPlayerInfo.fAttack = iAttack; } // 스킬의 경우 잠시 공격력을 올렸다가 내리도록 하기 위해서

	// UI 관련
	void				CObj_Manager::Set_Player_PlusExp(_float fExp) { m_tPlayerInfo.fExp += fExp; }	// TODO : 원래는 안 에서 함수 만들어서 해야하는데 이미 몬스터에서 각각 다 처리함..ㅋㅋ
	void				CObj_Manager::Set_Player_PlusHP(_float fHP);
	void				CObj_Manager::Set_Player_MinusHP(_float fAttack);

	void				Set_Coin(_int iCount) { m_tPlayerInfo.iCoin += iCount; }
	void				Set_Key() { m_tPlayerInfo.iKey += 1; }
	void				Set_Heart() { m_tPlayerInfo.iHeart += 1; }

	// UI Talk
	_bool				Get_Interaction() { return m_bInteraction; }

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
	void		CObj_Manager::Current_Player();		// 현재 플레이어가 누구인지 Tick
	void		CObj_Manager::Player_Exp(const _double & TimeDelta);			// 현재 플레이어의 경험치를 Tick
	void		CObj_Manager::Player_Weapon();		// 현재 플레이어의 무기 출력

private:
	void		CObj_Manager::Key_Input();

private:
	_bool			m_bNextLevel = true;	// 처음에 true 였다가 레벨이 시작하면 flase 로 초기화 한다. 그리고, 다음 맵을 넘어갈 때 true 가 된다.
	LEVEL			m_eCurrent_Level;
	PLAYERINFO		m_tPlayerInfo;

private: // KeyInput
	_bool			g_bNavigationRender = false;
	_bool			m_bInteraction = false;

private:
	_bool			m_bShield = false;
	_bool			m_bMonster_Crash = false;	// 플레이어가 지금 몬스터와 충돌 했는지

	_float			m_fAngle = 0.f;

private:
	// X 키를 누를 때 마다 Player 가 달라진다. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
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