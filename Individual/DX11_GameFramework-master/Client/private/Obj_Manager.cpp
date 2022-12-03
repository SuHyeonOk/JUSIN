#include "stdafx.h"
#include "..\public\Obj_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CObj_Manager)

CObj_Manager::CObj_Manager()
{
}

void CObj_Manager::Tick(_double TimeDelta)
{
	Current_Player();


}

void CObj_Manager::Current_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_X))
	{
		m_ChangeTarget++;
	}

	switch (m_ChangeTarget)
	{
	case FINN:
		m_eCurrent_Player = FINN;
		break;

	case JAKE:
		m_eCurrent_Player = JAKE;
		break;

	case FREE:
		m_eCurrent_Player = FREE; // ▣ 추 후 삭제 해야함
		break;

	case RESET:
		m_ChangeTarget = 0; // 즉, Finn
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CObj_Manager::Free()
{
}