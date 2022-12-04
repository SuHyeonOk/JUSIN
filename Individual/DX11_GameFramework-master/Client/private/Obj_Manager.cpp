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

void CObj_Manager::Free()
{
}