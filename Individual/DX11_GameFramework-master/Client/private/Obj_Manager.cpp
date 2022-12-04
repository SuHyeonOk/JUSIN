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
	Current_Player();			// 현재 플레이어가 누구인지 Tick

	// TODO : 어디서 Tick 을 돌려야 할지 모르겠음..!
	//Get_Player_Transform();		// 현재 플레이어의 좌표를 Tick
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
		m_ePlayerInfo.ePlayer = m_ePlayerInfo.FREE; // ▣ 추 후 삭제 해야함
		break;

	case PLAYERINFO::RESET:
		m_ChangeTarget = 0; // 즉, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

_vector			CObj_Manager::Get_Player_Transform()
{
	// 현재 Player 를 확인하고 그 Player 의 Transform 을 넘겨준다.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_ePlayerInfo.ePlayer == m_ePlayerInfo.FINN)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn 좌표 받아옴
		return vPlayerPos;
	}
	else if (m_ePlayerInfo.ePlayer == m_ePlayerInfo.JAKE)
	{
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(m_ePlayerInfo.ePlayer_Level, TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

		_vector vPlayerPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake 좌표 받아옴
		return vPlayerPos;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _vector();	// 없다면 쓰레기 값을 넘김
}

void		CObj_Manager::Free()
{
}