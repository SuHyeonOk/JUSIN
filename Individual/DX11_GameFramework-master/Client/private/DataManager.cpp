#include "stdafx.h"
#include "..\public\DataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
}

void			CDataManager::Clear_Environment()
{
	m_vecEnvironmentInfo.clear();
}

void			CDataManager::Set_Environment(const _tchar& ObjName, const _float3& ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecEnvironmentInfo.push_back(eObjInfo);
}

void CDataManager::Set_FoodInfo(const _tchar & ObjName, const _float3 & ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecFoodInfo.push_back(eObjInfo);
}

void CDataManager::Set_CoinInfo(const _tchar & ObjName, const _float3 & ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecCoinInfo.push_back(eObjInfo);
}

void CDataManager::Set_PageInfo(const _tchar & ObjName, const _float3 & ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecPageInfo.push_back(eObjInfo);
}

void CDataManager::Set_ItemInfo(const _tchar & ObjName, const _float3 & ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecItemInfo.push_back(eObjInfo);
}

void CDataManager::Set_ObjectInfo(const _tchar & ObjName, const _float3 & ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecObjectInfo.push_back(eObjInfo);
}

void CDataManager::Set_MonsterInfo(const _tchar & ObjName, const _float3 & ObjPos)
{
	OBJINFO		eObjInfo;
	memcpy(eObjInfo.ObjName, &ObjName, sizeof(_char[MAX_PATH]));
	eObjInfo.ObjPos = ObjPos;

	m_vecMonsterInfo.push_back(eObjInfo);
}

void CDataManager::Free()
{
	m_vecEnvironmentInfo.clear();
	m_vecFoodInfo.clear();
	m_vecCoinInfo.clear();
	m_vecPageInfo.clear();
	m_vecItemInfo.clear();
	m_vecObjectInfo.clear();
	m_vecMonsterInfo.clear();
}
