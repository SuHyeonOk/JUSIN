#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CObj_Manager)

CObj_Manager::CObj_Manager()
{
}

HRESULT		CObj_Manager::Initialized()
{
	m_tPlayerInfo.iHp		= 100;
	m_tPlayerInfo.iHpMax	= 100;
	m_tPlayerInfo.iAttack	= 10;
	m_tPlayerInfo.iExp		= 0;
	m_tPlayerInfo.iExpMax	= 100;
	m_tPlayerInfo.iLevel	= 1;
	m_tPlayerInfo.iKey		= 0;
	m_tPlayerInfo.iHeart	= 0;
	m_tPlayerInfo.iCoin		= 0;

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
	// ���� Player �� Ȯ���ϰ� �� Player �� Transform �� �Ѱ��ش�.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.FINN)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn ��ǥ �޾ƿ�
		return vPlayerPos;
	}
	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�
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

void		CObj_Manager::Set_Player_MinusHp(_int eHp)
{
	//if (PLAYERINFO::PLAYER::JAKE == m_tPlayerInfo.ePlayer &&
	//	PLAYERINFO::STATE::CONTROL == m_tPlayerInfo.eJakeWeapon)
	//	return;

	if (m_bShield)
	{
		m_bShield = false;
		return;
	}

	m_iMonster_Attck = eHp;
}

void		CObj_Manager::Tick(_double TimeDelta)
{
	Current_Player();			// ���� �÷��̾ ��������                                     Tick
	Player_Exp();				// �÷��̾� ����ġ�� ����Ͽ� ���� ����ġ ���� Ŀ���� ������, �ִ� ����ġ ����, ���ݷ� ����
	Key_Input();				// Npc ���� ��ȭâ

	//cout << "HP : " << m_tPlayerInfo.iHp << " | MAXHP : " << m_tPlayerInfo.iHpMax <<
	//	" | ATTACK : " << m_tPlayerInfo.iAttack << " | LEVEL : " << m_tPlayerInfo.iLevel << 
	//	" | EXP : " << m_tPlayerInfo.iExp << " | MAXEXP : " << m_tPlayerInfo.iExpMax << endl;

	if (0 < m_iMonster_Attck)
	{
		m_dPlayerAttck_TimeAcc += TimeDelta;
		if (0.7 < m_dPlayerAttck_TimeAcc)
		{
			if (0 < m_tPlayerInfo.iHp) m_tPlayerInfo.iHp -= m_iMonster_Attck;
			
			m_iMonster_Attck = 0;
			m_dPlayerAttck_TimeAcc = 0;
		}
	}

}

void		CObj_Manager::Key_Input()
{
	// TODO : ���⺯��
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (1 == pGameInstance->Key_OneUpDwon(DIK_0))
		cout << "1" << endl;
	if (0 == pGameInstance->Key_OneUpDwon(DIK_0))
		cout << "0" << endl;

	if (pGameInstance->Key_Down(DIK_V))
		m_bIsTalk = true;
	if (pGameInstance->Key_Up(DIK_B))
		m_bIsTalk = false;

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
	if (m_tPlayerInfo.iExp >= m_tPlayerInfo.iExpMax)
	{
		m_tPlayerInfo.iHp = m_tPlayerInfo.iHpMax;		// ü�� �� ä���ֱ�
		m_tPlayerInfo.iLevel++;							// ���� ����
		m_tPlayerInfo.iExp = 0;							// ����ġ 0 ���� �ʱ�ȭ
		m_tPlayerInfo.iExpMax += 50;					// �ִ� ����ġ ����
		m_tPlayerInfo.iAttack += 10;					// ���ݷ� ����

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
