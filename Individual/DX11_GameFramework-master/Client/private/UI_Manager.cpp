#include "stdafx.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

void	CUI_Manager::Add_Text(TEXTINFO::NPCTEXT eNpcText, const _tchar & cText)
{
	//TEXTINFO		tTextInfo;
	//memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	//eObjInfo.ObjPos = ObjPos;

	//m_vecFoodInfo.push_back(eObjInfo);
}

void CUI_Manager::Free()
{
}