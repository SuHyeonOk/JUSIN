#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"
#include "Transform.h"
#include "UI_Manager.h"
#include "Skill_Manager.h"
#include "Effect_Manager.h"

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

	// ���� Player �� Ȯ���ϰ� �� Player �� Transform �� �Ѱ��ش�.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINN)
	{
		CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn ��ǥ �޾ƿ�
		return vPlayerPos;
	}
	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�
		return vPlayerPos;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _vector();	// ���ٸ� ������ ���� �ѱ�
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
		_float fPlayerHp = m_tPlayerInfo.fHP + fHP;		// ���� ü�� + �߰��� HP �� �����

		if (m_tPlayerInfo.fHPMax < fPlayerHp)			// ��ü HPMax ���� ũ�ٸ�
			m_tPlayerInfo.fHP = m_tPlayerInfo.fHPMax;	// �׳� HPMax ������ ä���, 
		else
			m_tPlayerInfo.fHP += fHP;					// �� �ܴ� �߰��� HP ��ŭ ü���� �߰��ȴ�.
	}

	CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
}

void		CObj_Manager::Set_Player_MinusHP(_float fAttack)
{
	if (m_bShield)	// Jake �� ���� �� true ��� �÷��̾�� ���� ���� �ʴ´�.
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

	Current_Player();			// ���� �÷��̾ �������� Tick
	Player_Exp(TimeDelta);		// �÷��̾� ����ġ�� ����Ͽ� ���� ����ġ ���� Ŀ���� ������, �ִ� ����ġ ����, ���ݷ� ����
	Key_Input();				// ��ü���� Ű �Է�
	Player_Weapon();			// ���� �÷��̾��� ���⸦ ����Ѵ�.

	//cout << "HP : " << m_tPlayerInfo.fHP << " / " << m_tPlayerInfo.fHPMax << " | HP% : " << m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax <<
	//	" | ATTACK : " << m_tPlayerInfo.iAttack << " | LEVEL : " << m_tPlayerInfo.iLevel << 
	//	" | EXP : " << m_tPlayerInfo.fExp << " / : " << m_tPlayerInfo.fExpMax << " | EXP% : " << m_tPlayerInfo.fExp / m_tPlayerInfo.fExpMax <<
	//	" | KEY : " << m_tPlayerInfo.iKey << " | COIN : " << m_tPlayerInfo.iCoin << " | HEART: " << m_tPlayerInfo.iHeart << endl;

	//cout << CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill << endl;

	// �÷��̾� ü�� ���� ���� ���� ���� ������ ��
	if (0 < m_fMonster_Attck)
	{
		if (0 == m_dPlayerAttck_TimeAcc && 0 < m_tPlayerInfo.fHP)
		{
			m_tPlayerInfo.fHP -= m_fMonster_Attck;
			CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
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

	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill && // ������ ����� SKILL_END �� ���� ����� �� �ִ�.
		false == m_bInventory) // �κ��丮�� �ݰ� ���� ���� ����� �� �ִ�.
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

	// ���������� ���̰��̼�, �浹ü�� ���ٰ� �� �� �ִ�.
	if (pGameInstance->Key_Down(DIK_F11))
		g_bNavigationRender = !g_bNavigationRender;

	// Inventory
	if (pGameInstance->Key_Down(DIK_X))
	{
		m_bInteraction = !m_bInteraction;
		m_bInventory = !m_bInventory;
	}

	// ��ȣ�ۿ� Ű �������� ����.
	if (pGameInstance->Key_Down(DIK_F))
		m_bInteraction = !m_bInteraction;

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
		if (pGameInstance->Key_Down(DIK_Z))		// �÷��̾ ���� �Ǵ� ��ų ����� �� ���� �������� �� �Ѵ�.
		{
			m_ChangeTarget++;
			Set_Current_Player_State(PLAYERINFO::CHANGE);
		}
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
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FREE; // �� �� �� ���� �ؾ���
		break;
#endif

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // ��, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Player_Exp(const _double & TimeDelta)
{
	if (m_tPlayerInfo.fExp >= m_tPlayerInfo.fExpMax)
	{
		m_bEffect = true;

		m_tPlayerInfo.iLevel++;							// ���� ����
		m_tPlayerInfo.fHPMax += 10.0f;					// ���ݷ� ����
		m_tPlayerInfo.fHP = m_tPlayerInfo.fHPMax;		// ü�� �� ä���ֱ�
		m_tPlayerInfo.fExp = 0.0f;						// ����ġ 0 ���� �ʱ�ȭ
		m_tPlayerInfo.fExpMax += 50.0f;					// �ִ� ����ġ ����
		m_tPlayerInfo.fAttack += 10.0f;					// ���ݷ� ����

		// UI �ʱ�ȭ
		CUI_Manager::GetInstance()->Set_Level_Number(m_tPlayerInfo.iLevel);
		CUI_Manager::GetInstance()->Set_HPGauge_Player(m_tPlayerInfo.fHP / m_tPlayerInfo.fHPMax);
		CUI_Manager::GetInstance()->Set_LevelGauge_Player(m_tPlayerInfo.fExp / m_tPlayerInfo.fExpMax);
	}

	// ����Ʈ
	if (m_bEffect)
	{
		// �÷��̾� ���� ����
		m_tPlayerInfo.eState = PLAYERINFO::STATE::LEVEL_UP;

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
	// ������ �ϴ� UI
	if (CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
		CUI_Manager::GetInstance()->Set_Weapons(0);		// �⺻ ���� ��� ��
		CUI_Manager::GetInstance()->Set_Weapon_index(m_iWeapon_Index);
	}
	else
		CUI_Manager::GetInstance()->Set_Weapons(1);		// ��ų ��� ��					

	// ���� (��ų�� ������� ���� ���� �ѱ��.)
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
	else	// �ݴ�� ��ų�� ����� �̶�� ��ų�� �ε����� �ѱ��.
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

