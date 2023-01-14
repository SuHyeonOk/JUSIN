#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CDataManager : public CBase
{
	DECLARE_SINGLETON(CDataManager)

public:
	typedef struct tagObjectInfo
	{
		_tchar		ObjName[MAX_PATH];
		_float3		ObjPos;

	}OBJINFO;
	
public:
	CDataManager();
	virtual ~CDataManager() = default;

public:
	vector<OBJINFO>	CDataManager::Get_FoodInfo() { return m_vecFoodInfo; }
	vector<OBJINFO>	CDataManager::Get_CoinInfo() { return m_vecCoinInfo; }
	vector<OBJINFO>	CDataManager::Get_PageInfo() { return m_vecPageInfo; }
	vector<OBJINFO>	CDataManager::Get_ItemInfo() { return m_vecItemInfo; }
	vector<OBJINFO>	CDataManager::Get_ObjectInfo() { return m_vecObjectInfo; }
	vector<OBJINFO>	CDataManager::Get_MonsterInfo() { return m_vecMonsterInfo; }

	void			CDataManager::Set_FoodInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			CDataManager::Set_CoinInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			CDataManager::Set_PageInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			CDataManager::Set_ItemInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			CDataManager::Set_ObjectInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			CDataManager::Set_MonsterInfo(const _tchar& ObjName, const _float3& ObjPos);

private:
	vector<OBJINFO>	m_vecFoodInfo;
	vector<OBJINFO>	m_vecCoinInfo;
	vector<OBJINFO>	m_vecPageInfo;
	vector<OBJINFO>	m_vecItemInfo;
	vector<OBJINFO> m_vecObjectInfo;
	vector<OBJINFO>	m_vecMonsterInfo;

public:
	virtual void Free() override;
};

END