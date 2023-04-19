#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"
#include "Transform.h"
#include "UI_Manager.h"
#include "Skill_Manager.h"
#include "Effect_Manager.h"
#include "Utilities_Manager.h"

IMPLEMENT_SINGLETON(CObj_Manager)

CObj_Manager::CObj_Manager()
{
}

HRESULT		CObj_Manager::Initialized()
{
	m_tPlayerInfo.fHP = 100.0f;
	m_tPlayerInfo.fHPMax = 100.0f;
	m_tPlayerInfo.fAttack = 10.0f;
	m_tPlayerInfo.fExp = 0.0f;
	m_tPlayerInfo.fExpMax = 100.0f;
	m_tPlayerInfo.iLevel = 1;
	m_tPlayerInfo.iKey = 0;
	m_tPlayerInfo.iHeart = 0;
	m_tPlayerInfo.iCoin = 0;

	m_tPlayerInfo.iKey = 0;
	m_tPlayerInfo.iHeart = 0;

	m_tPlayerInfo.ePlayer = PLAYERINFO::PLAYER::FINN;
	m_tPlayerInfo.eState = PLAYERINFO::STATE::IDLE;
	m_tPlayerInfo.ePreState = PLAYERINFO::STATE::STATE_END;
	m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_DOLDEN;
	m_tPlayerInfo.eJakeWeapon = PLAYERINFO::JAKEWEAPON::LFIST;

	CUI_Manager::GetInstance()->Set_Level_Number(m_tPlayerInfo.iLevel);

	return S_OK;
}

_vector		CObj_Manager::Get_Player_Transform()
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
	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINNANDJAKE)
	{
		CTransform * pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_MINIGAME, TEXT("Layer_FinnAndJake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos = pTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn 과 Jake 좌표 받아옴
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

void		CObj_Manager::Set_Player_PlusHP(_float fHP)
{
	if (m_tPlayerInfo.fHP <= m_tPlayerInfo.fHPMax)
	{
		_float fPlayerHp = m_tPlayerInfo.fHP + fHP;		// 현재 체력 + 추가할 HP 의 결과가

		if (m_tPlayerInfo.fHPMax < fPlayerHp)			// 전체 HPMax 보다 크다면
			m_tPlayerInfo.fHP = m_tPlayerInfo.fHPMax;	// 그냥 HPMax 값으로 채우고, 
		else
			m_tPlayerInfo.fHP += fHP;					// 그 외는 추가한 HP 만큼 체력이 추가된다.
	}

	CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
}

void		CObj_Manager::Set_Player_MinusHP(_float fAttack)
{
	if (m_bShield)	// Jake 의 쉴드 가 true 라면 플레이어는 공격 받지 않는다.
	{
		m_bShield = false;
		return;
	}

	m_fMonster_Attck = fAttack;
}

void		CObj_Manager::Tick_Player_Transform()
{
	Get_Player_Transform();
}

void		CObj_Manager::Tick(_double TimeDelta)
{
	if (m_bNextLevel)
		return;

	Current_Player();			// 현재 플레이어가 누구인지 Tick
	Player_Exp(TimeDelta);		// 플레이어 경험치를 계산하여 일정 경험치 보다 커지면 레벨업, 최대 경험치 증가, 공격력 증가
	Key_Input();				// 전체적인 키 입력
	Player_Weapon();			// 현재 플레이어의 무기를 출력한다.

	//cout << "HP : " << m_tPlayerInfo.fHP << " / " << m_tPlayerInfo.fHPMax << " | HP% : " << m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax <<
	//	" | ATTACK : " << m_tPlayerInfo.iAttack << " | LEVEL : " << m_tPlayerInfo.iLevel << 
	//	" | EXP : " << m_tPlayerInfo.fExp << " / : " << m_tPlayerInfo.fExpMax << " | EXP% : " << m_tPlayerInfo.fExp / m_tPlayerInfo.fExpMax <<
	//	" | KEY : " << m_tPlayerInfo.iKey << " | COIN : " << m_tPlayerInfo.iCoin << " | HEART: " << m_tPlayerInfo.iHeart << endl;

	//cout << CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill << endl;

	// 플레이어 체력 관리 몬스터 에게 공격 당했을 때
	if (0 < m_fMonster_Attck)
	{
		if (0 == m_dPlayerAttck_TimeAcc && 0 < m_tPlayerInfo.fHP)
		{
			m_tPlayerInfo.fHP -= m_fMonster_Attck;
			CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);

			// 사운드
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			_tchar szSoundName[MAX_PATH];

			if (PLAYERINFO::PLAYER::FINN == m_tPlayerInfo.ePlayer)
			{
				wsprintf(szSoundName, TEXT("v_Finn_Hit%d.ogg"), CUtilities_Manager::GetInstance()->Get_Random(1, 3));
				pGameInstance->Play_Sound(szSoundName, 0.7f);
			}
			else
			{
				wsprintf(szSoundName, TEXT("v_Jake_Hit%d.ogg"), CUtilities_Manager::GetInstance()->Get_Random(1, 3));
				pGameInstance->Play_Sound(szSoundName, 0.7f);
			}

			RELEASE_INSTANCE(CGameInstance);
		}

		m_dPlayerAttck_TimeAcc += TimeDelta;
		if (0.7 < m_dPlayerAttck_TimeAcc)
		{
			m_fMonster_Attck = 0;
			m_dPlayerAttck_TimeAcc = 0;
		}
	}
}

void		CObj_Manager::Key_Input()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill && // 아이템 사용은 SKILL_END 일 때만 사용할 수 있다.
		false == m_bInventory) // 인벤토리를 닫고 있을 때만 사용할 수 있다.
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

	// Inventory
	if (pGameInstance->Key_Down(DIK_X))
	{
		pGameInstance->Play_Sound(TEXT("sfx_map_open.ogg"), 0.7f);

		m_bInteraction = !m_bInteraction;
		m_bInventory = !m_bInventory;
	}

	// 상호작용 키 움직이지 못함.
	if (pGameInstance->Key_Down(DIK_F))
		m_bInteraction = !m_bInteraction;

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
		if (pGameInstance->Key_Down(DIK_Z))		// 플레이어가 변신 또는 스킬 사용중 일 때는 변경하지 못 한다.
		{
			if (CObj_Manager::PLAYERINFO::PLAYER::FINN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
				pGameInstance->Play_Sound(TEXT("v_Jake_Swap.ogg"), 1.0f);
			else
				pGameInstance->Play_Sound(TEXT("v_Finn_Swap.ogg"), 1.0f);

			m_ChangeTarget++;
			Set_Current_Player_State(PLAYERINFO::CHANGE);
		}
	}

	if (CObj_Manager::PLAYERINFO::PLAYER::CUTSCENE_ONE == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer ||
		CObj_Manager::PLAYERINFO::PLAYER::CUTSCENE_TWO == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer ||
		CObj_Manager::PLAYERINFO::PLAYER::FINNANDJAKE == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
		
	switch (m_ChangeTarget)
	{
	case PLAYERINFO::FINN:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FINN;
		break;

	case PLAYERINFO::JAKE:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.JAKE;
		break;

#ifdef F2_SKELETON
	case PLAYERINFO::FREE:
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FREE; // ▣ 추 후 삭제 해야함
		break;
#endif

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // 즉, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Player_Exp(const _double & TimeDelta)
{
	if (m_tPlayerInfo.fExp >= m_tPlayerInfo.fExpMax)
	{
		m_bEffect = true;
		m_bLevelUP = true;
	}

	if (true == m_bLevelUP)
	{
		m_bLevelUP = false;

		m_tPlayerInfo.iLevel++;									// 레벨 증가
		m_tPlayerInfo.fHPMax += 10.0f;					// 공격력 증가
		m_tPlayerInfo.fHP = m_tPlayerInfo.fHPMax;		// 체력 꽉 채워주기
		m_tPlayerInfo.fExp = 0.0f;								// 경험치 0 으로 초기화
		m_tPlayerInfo.fExpMax += 100.0f;				// 최대 경험치 증가
		m_tPlayerInfo.fAttack += 10.0f;					// 공격력 증가

														// UI 초기화
		CUI_Manager::GetInstance()->Set_Level_Number(m_tPlayerInfo.iLevel);
		CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
		CUI_Manager::GetInstance()->Set_LevelGauge_Player(m_tPlayerInfo.fExp / m_tPlayerInfo.fExpMax);
	}

	// 이펙트
	if (m_bEffect)
	{
		// 플레이어 상태 변경
		m_tPlayerInfo.eState = PLAYERINFO::STATE::LEVEL_UP;

		if (0 == m_dEffect_Up_TimeAcc)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("sfx_character_levelup.ogg"), 0.7f);
			RELEASE_INSTANCE(CGameInstance);
		}

		m_dEffect_Up_TimeAcc += TimeDelta;
		if (0.35 < m_dEffect_Up_TimeAcc)
		{
			_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
			_float4 f4PlayerPos;
			XMStoreFloat4(&f4PlayerPos, vPlayerPos);

			CEffect_Manager::GetInstance()->Food_Up(_float3(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z));
			CEffect_Manager::GetInstance()->Beneficial(_float3(f4PlayerPos.x, 0.7f, f4PlayerPos.z), _float3(0.9f, 1.0f, 0.6f));
			CEffect_Manager::GetInstance()->Effect_StarRandom_Create(_float3(f4PlayerPos.x, f4PlayerPos.y + 1.0f, f4PlayerPos.z - 1.0f), _float3(0.9f, 1.0f, 0.6f));
			m_dEffect_Up_TimeAcc = 0;
		}

 		m_dEffect_TimeAcc += TimeDelta;
		if (3 < m_dEffect_TimeAcc)
		{
			m_bEffect = false;
			m_dEffect_TimeAcc = 0;
			return;
		}
	}
}

void		CObj_Manager::Player_Weapon()
{
	// 오른쪽 하단 UI
	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
		CUI_Manager::GetInstance()->Set_Weapons(0);		// 기본 공격 사용 중
		CUI_Manager::GetInstance()->Set_Weapon_index(m_iWeapon_Index);
	}
	else
		CUI_Manager::GetInstance()->Set_Weapons(1);		// 스킬 사용 중					

	// 무기 (스킬을 사용하지 않을 때만 넘긴다.)
	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
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
	else	// 반대로 스킬을 사용중 이라면 스킬의 인덱스를 넘긴다.
	{
		m_iWeapon_Index = CUI_Manager::GetInstance()->Get_Weapon_index();

		//m_iWeapon_Index = CUI_Manager::GetInstance()->Get_Weapon_index();

		if (CSkill_Manager::PLAYERSKILL::PAINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CUI_Manager::GetInstance()->Set_Weapon_index(4);
		else if (CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CUI_Manager::GetInstance()->Set_Weapon_index(5);
		else if (CSkill_Manager::PLAYERSKILL::COIN == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CUI_Manager::GetInstance()->Set_Weapon_index(6);
		else if (CSkill_Manager::PLAYERSKILL::FIONA == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CUI_Manager::GetInstance()->Set_Weapon_index(7);
		else if (CSkill_Manager::PLAYERSKILL::FOOD == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CUI_Manager::GetInstance()->Set_Weapon_index(10);
		else if (CSkill_Manager::PLAYERSKILL::JAKESON == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CUI_Manager::GetInstance()->Set_Weapon_index(11);
	}
}

void		CObj_Manager::Free()
{
}

