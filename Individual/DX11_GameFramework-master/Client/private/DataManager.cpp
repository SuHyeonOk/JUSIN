#include "stdafx.h"
#include "..\public\DataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
}

void CDataManager::Set_FoodInfo(const _tchar & ObjName, const _float3 & ObjPos, void * pArg)
{
	OBJINFO		eObjInfo;
	eObjInfo.ObjName[MAX_PATH] = ObjName;
	eObjInfo.ObjPos = ObjPos;

	m_vecFoodInfo.push_back(eObjInfo);
}

void CDataManager::Free()
{
}
