#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"
#include "Transform.h"
#include "UI_Manager.h"

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
	m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_ROOT;
	m_tPlayerInfo.eJakeWeapon = PLAYERINFO::JAKEWEAPON::LFIST;



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

//_bool	CObj_Manager::Get_Player_Collider(CCollider* pColliderCom[COLLTYPE_END])
//{
//	// ���� Player �� Ȯ���ϰ�, �� Player �� Collider �� �Ѱ��ش�.
//
//	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
//
//	if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINN)
//	{
//		CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Collider"), 0);
//		if (nullptr == pTargetCollider)
//			return false;
//
//		return pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider);
//	}
//	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
//	{
//		CCollider*		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Collider"), 0);
//		if (nullptr == pTargetCollider)
//			return false;
//
//		return pColliderCom[COLLTYPE_AABB]->Collision(pTargetCollider);
//	}
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return false;	// ���ٸ� ������ ���� �ѱ�
//}

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
	Current_Player();			// ���� �÷��̾ ��������                                     Tick
	Player_Exp();				// �÷��̾� ����ġ�� ����Ͽ� ���� ����ġ ���� Ŀ���� ������, �ִ� ����ġ ����, ���ݷ� ����
	Key_Input();				// ��ü���� Ű �Է�

	//cout << "HP : " << m_tPlayerInfo.iHp << " | MAXHP : " << m_tPlayerInfo.iHpMax <<
	//	" | ATTACK : " << m_tPlayerInfo.iAttack << " | LEVEL : " << m_tPlayerInfo.iLevel << 
	//	" | EXP : " << m_tPlayerInfo.iExp << " | MAXEXP : " << m_tPlayerInfo.iExpMax << endl;

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

	// ���������� ���̰��̼�, �浹ü�� ���ٰ� �� �� �ִ�.
	if (pGameInstance->Key_Down(DIK_F11))
		g_bNavigationRender = !g_bNavigationRender;

	// ��ȣ�ۿ� Ű �������� ����.
	if (pGameInstance->Key_Down(DIK_F))
		m_bInteraction = !m_bInteraction;

	// TODO : ���⺯��
	if (pGameInstance->Key_Down(DIK_U))	// TODO : 1 Map �� ������ ����
	{
		m_tPlayerInfo.ePlayerWeapon = PLAYERINFO::PLAYERWEAPON::F_DOLDEN;
	}
	if (pGameInstance->Key_Down(DIK_I))	// TODO : �߰� ���� ������ ����
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
		m_tPlayerInfo.ePlayer = m_tPlayerInfo.FREE; // �� �� �� ���� �ؾ���
		break;

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // ��, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void		CObj_Manager::Player_Exp()
{
	if (m_tPlayerInfo.fExp >= m_tPlayerInfo.fExpMax)
	{
		m_tPlayerInfo.fHP = m_tPlayerInfo.fHPMax;		// ü�� �� ä���ֱ�
		m_tPlayerInfo.iLevel++;							// ���� ����
		m_tPlayerInfo.fExp = 0.0f;							// ����ġ 0 ���� �ʱ�ȭ
		m_tPlayerInfo.fExpMax += 50.0f;					// �ִ� ����ġ ����
		m_tPlayerInfo.iAttack += 10;					// ���ݷ� ����
		m_tPlayerInfo.fHPMax += 10.0f;					// ���ݷ� ����

		return;
	}
}

_float		CObj_Manager::Get_Player_Distance(_fvector	_MyPos)
{
	_vector	vDistance = CObj_Manager::GetInstance()->Get_Player_Transform() - _MyPos;

	return XMVectorGetX(XMVector3Length(vDistance));
}

void		CObj_Manager::Free()
{
}
