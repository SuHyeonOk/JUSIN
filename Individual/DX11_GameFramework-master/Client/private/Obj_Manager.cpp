#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"
#include "Transform.h"
#include "UI_Manager.h"
#include "Skill_Manager.h"

IMPLEMENT_SINGLETON(CObj_Manager)

CObj_Manager::CObj_Manager()
{
}

HRESULT		CObj_Manager::Initialized()
{
	m_tPlayerInfo.fHP		= 100.0f;
	m_tPlayerInfo.fHPMax	= 100.0f;
	m_tPlayerInfo.iAttack	= 10;
	m_tPlayerInfo.fExp		= 0.0f;
	m_tPlayerInfo.fExpMax	= 100.0f;
	m_tPlayerInfo.iLevel	= 1;
	m_tPlayerInfo.iKey		= 0;
	m_tPlayerInfo.iHeart	= 0;
	m_tPlayerInfo.iCoin		= 0;

	m_tPlayerInfo.iKey = 0;
	m_tPlayerInfo.iHeart = 0;

	m_tPlayerInfo.ePlayer = PLAYERINFO::PLAYER::FINN;
	m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_DOLDEN;
	m_tPlayerInfo.eJakeWeapon = PLAYERINFO::JAKEWEAPON::LFIST;

	CUI_Manager::GetInstance()->Set_Level_Number(m_tPlayerInfo.iLevel);

	return S_OK;
}

void CObj_Manager::Tick_Player_Transform()
{
	Get_Player_Transform();
}

_vector			CObj_Manager::Get_Player_Transform()
{
	if (m_bNextLevel)
		return _vector();

	// 현재 Player 를 확인하고 그 Player 의 Transform 을 넘겨준다.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINN)
	{
		CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn 좌표 받아옴
		return vPlayerPos;
	}
	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake 좌표 받아옴
		return vPlayerPos;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _vector();	// 없다면 쓰레기 값을 넘김
}

_float		CObj_Manager::Get_Player_Distance(_fvector	_MyPos)
{
	_vector	vDistance = CObj_Manager::GetInstance()->Get_Player_Transform() - _MyPos;

	return XMVectorGetX(XMVector3Length(vDistance));
}

void		CObj_Manager::Set_Player_MinusHP(_float fAttack)
{
	if (m_bShield)
	{
		m_bShield = false;
		return;
	}

	m_fMonster_Attck = fAttack;
}

void		CObj_Manager::Tick(_double TimeDelta)
{
	Current_Player();			// 현재 플레이어가 누구인지                                     Tick
	Player_Exp();				// 플레이어 경험치를 계산하영 일정 경험치 보다 커지면 레벨업, 최대 경험치 증가, 공격력 증가
	Key_Input();				// 전체적인 키 입력
	Player_Weapon();			// 현재 플레이어의 무기를 출력한다.

	//cout << "HP : " << m_tPlayerInfo.fHP << " / " << m_tPlayerInfo.fHPMax << " | HP% : " << m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax <<
	//	" | ATTACK : " << m_tPlayerInfo.iAttack << " | LEVEL : " << m_tPlayerInfo.iLevel << 
	//	" | EXP : " << m_tPlayerInfo.fExp << " / : " << m_tPlayerInfo.fExpMax << " | EXP% : " << m_tPlayerInfo.fExp / m_tPlayerInfo.fExpMax <<
	//	" | KEY : " << m_tPlayerInfo.iKey << " | COIN : " << m_tPlayerInfo.iCoin << " | HEART: " << m_tPlayerInfo.iHeart << endl;

	if (0 < m_fMonster_Attck)
	{
		m_dPlayerAttck_TimeAcc += TimeDelta;
		if (0.7 < m_dPlayerAttck_TimeAcc)
		{
			if (0 < m_tPlayerInfo.fHP)
			{
				m_tPlayerInfo.fHP -= m_fMonster_Attck;
				CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
			}
			
			m_fMonster_Attck = 0;
			m_dPlayerAttck_TimeAcc = 0;
		}
	}
}

void		CObj_Manager::Key_Input()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 아이템 사용은 SKILL_END 일 때만 사용할 수 있다.
	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
		if (pGameInstance->Key_Down(DIK_1))
			CSkill_Manager::GetInstance()->Page_Use(ITEM_ONE);
		if (pGameInstance->Key_Down(DIK_2))
			CSkill_Manager::GetInstance()->Page_Use(ITEM_TWO);
		if (pGameInstance->Key_Down(DIK_3))
			CSkill_Manager::GetInstance()->Page_Use(ITEM_THREE);
		if (pGameInstance->Key_Down(DIK_4))
			CSkill_Manager::GetInstance()->Page_Use(ITEM_FOUR);
	}

	// 전역적으로 네이게이션, 충돌체를 껐다가 켤 수 있다.
	if (pGameInstance->Key_Down(DIK_F11))
		g_bNavigationRender = !g_bNavigationRender;

	// 상호작용 키 움직이지 못함.
	if (pGameInstance->Key_Down(DIK_F))
		m_bInteraction = !m_bInteraction;

	// TODO : 무기변경
	if (pGameInstance->Key_Down(DIK_U))	// TODO : 1 Map 이 끝나면 변경
	{
		m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_ROOT;
	}
	if (pGameInstance->Key_Down(DIK_I))	// TODO : 중간 보스 잡으면 변경
	{
		m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_FAMILY;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_Z))
	{
		m_ChangeTarget++;
		Set_Current_Player_State(PLAYERINFO::CHANGE);
	}

	switch (m_ChangeTarget)
	{
	case PLAYERINFO::FINN:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FINN;
		break;

	case PLAYERINFO::JAKE:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.JAKE;
		break;

	case PLAYERINFO::FREE:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FREE; // ▣ 추 후 삭제 해야함
		break;

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // 즉, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Player_Exp()
{
	if (m_tPlayerInfo.fExp >= m_tPlayerInfo.fExpMax)
	{
		m_tPlayerInfo.iLevel++;							// 레벨 증가
		m_tPlayerInfo.fHPMax += 10.0f;					// 공격력 증가
		m_tPlayerInfo.fHP = m_tPlayerInfo.fHPMax;		// 체력 꽉 채워주기
		m_tPlayerInfo.fExp = 0.0f;						// 경험치 0 으로 초기화
		m_tPlayerInfo.fExpMax += 50.0f;					// 최대 경험치 증가
		m_tPlayerInfo.iAttack += 10;					// 공격력 증가

		// UI 초기화
		CUI_Manager::GetInstance()->Set_Level_Number(m_tPlayerInfo.iLevel);
		CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
		CUI_Manager::GetInstance()->Set_LevelGauge_Player(m_tPlayerInfo.fExp / m_tPlayerInfo.fExpMax);

		return;
	}
}

void		CObj_Manager::Player_Weapon()
{
	// 스킬 사용? 무기사용?
	if(CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		CUI_Manager::GetInstance()->Set_Weapons(0);
	else
		CUI_Manager::GetInstance()->Set_Weapons(1);

	// 무기
	if (CObj_Manager::PLAYERINFO::FINN == m_tPlayerInfo.ePlayer)
	{
		if (CObj_Manager::PLAYERINFO::F_DOLDEN == m_tPlayerInfo.ePlayerWeapon)
			CUI_Manager::GetInstance()->Set_Weapon_index(0);
		else if (CObj_Manager::PLAYERINFO::F_ROOT == m_tPlayerInfo.ePlayerWeapon)
			CUI_Manager::GetInstance()->Set_Weapon_index(1);
		else
			CUI_Manager::GetInstance()->Set_Weapon_index(2);
	}
	else
	{
		CUI_Manager::GetInstance()->Set_Weapon_index(3);
	}
}

void		CObj_Manager::Free()
{
}
