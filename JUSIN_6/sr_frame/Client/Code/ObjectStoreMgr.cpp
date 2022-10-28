#include "stdafx.h"
#include "..\Header\ObjectStoreMgr.h"

IMPLEMENT_SINGLETON(CObjectStoreMgr)

CObjectStoreMgr::CObjectStoreMgr()
{
}

CObjectStoreMgr::~CObjectStoreMgr()
{
}

void CObjectStoreMgr::SetPlayerSkin(CharacterSelect eSkin)
{
	switch (eSkin)
	{
	case CHAR_STEVE:
		m_eSkin = Protocol::PLAYER_TYPE_STEVE;
		break;
	case CHAR_ESHE:
		m_eSkin = Protocol::PLAYER_TYPE_ESHE;
		break;
	case CHAR_COPPER:
		m_eSkin = Protocol::PLAYER_TYPE_COPPER;
		break;
	case CHAR_PRIDE:
		m_eSkin = Protocol::PLAYER_TYPE_PRIDE;
		break;
	case CHAR_END: 
		break;
	default: ;
	}
}

void CObjectStoreMgr::Free()
{
	Safe_Release(m_pPlayerInven);
}
