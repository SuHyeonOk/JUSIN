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
	m_ePlayerInfo.iHp		= 100;
	m_ePlayerInfo.iAttack	= 10;
	m_ePlayerInfo.iExp		= 0;
	m_ePlayerInfo.iLevel	= 0;
	m_ePlayerInfo.iKey		= 0;
	m_ePlayerInfo.iHeart	= 0;

	return S_OK;
}

void		CObj_Manager::Tick(_double TimeDelta)
{
	Current_Player();			// ���� �÷��̾ �������� Tick

	// TODO : ��� Tick �� ������ ���� �𸣰���..!
	//Get_Player_Transform();		// ���� �÷��̾��� ��ǥ�� Tick
}

void		CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_X))
		m_ChangeTarget++;

	switch (m_ChangeTarget)
	{
	case PLAYERINFO::FINN:
		m_ePlayerInfo.ePlayer = m_ePlayerInfo.FINN;
		break;

	case PLAYERINFO::JAKE:
		m_ePlayerInfo.ePlayer = m_ePlayerInfo.JAKE;
		break;

	case PLAYERINFO::FREE:
		m_ePlayerInfo.ePlayer = m_ePlayerInfo.FREE; // �� �� �� ���� �ؾ���
		break;

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // ��, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

_vector			CObj_Manager::Get_Player_Transform()
{
	// ���� Player �� Ȯ���ϰ� �� Player �� Transform �� �Ѱ��ش�.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_ePlayerInfo.ePlayer == m_ePlayerInfo.FINN)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn ��ǥ �޾ƿ�
		return vPlayerPos;
	}
	else if (m_ePlayerInfo.ePlayer == m_ePlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(m_ePlayerInfo.ePlayer_Level, TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�
		return vPlayerPos;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _vector();	// ���ٸ� ������ ���� �ѱ�
}

void		CObj_Manager::Free()
{
}