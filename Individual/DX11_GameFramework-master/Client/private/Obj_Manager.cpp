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
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(m_tPlayerInfo.ePlayer_Level, TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn ��ǥ �޾ƿ�
		return vPlayerPos;
	}
	else if (m_tPlayerInfo.ePlayer == m_tPlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(m_tPlayerInfo.ePlayer_Level, TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�
		return vPlayerPos;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _vector();	// ���ٸ� ������ ���� �ѱ�
}

void		CObj_Manager::Tick(_double TimeDelta)
{
	Current_Player();			// ���� �÷��̾ �������� Tick
	Player_Exp();				// �÷��̾� ����ġ�� ����Ͽ� ���� ����ġ ���� Ŀ���� ������, �ִ� ����ġ ����, ���ݷ� ����

	// TODO : ��� Tick �� ������ ���� �𸣰���..!
}

void		CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_X))
		m_ChangeTarget++;

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
		m_tPlayerInfo.iLevel++;			// ���� ����
		m_tPlayerInfo.iExpMax += 50;	// �ִ� ����ġ ����
		m_tPlayerInfo.iAttack += 20;	// ���ݷ� ����
		cout << m_tPlayerInfo.iLevel << endl;
		return;
	}
}

void		CObj_Manager::Free()
{
}
