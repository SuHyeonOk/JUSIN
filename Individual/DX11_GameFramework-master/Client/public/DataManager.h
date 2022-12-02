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

		void Set_CoinInfo(const _tchar & ObjName, const _float3 & ObjPos);

	}OBJINFO;
	
public:
	CDataManager();
	virtual ~CDataManager() = default;

public:
	vector<OBJINFO>	Get_FoodInfo() { return m_vecFoodInfo; }
	vector<OBJINFO>	Get_CoinInfo() { return m_vecCoinInfo; }
	vector<OBJINFO>	Get_PageInfo() { return m_vecPageInfo; }
	vector<OBJINFO>	Get_MonsterInfo() { return m_vecMonsterInfo; }

	void			Set_FoodInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			Set_CoinInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			Set_PageInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			Set_MonsterInfo(const _tchar& ObjName, const _float3& ObjPos);

private:
	vector<OBJINFO>	m_vecFoodInfo;
	vector<OBJINFO>	m_vecCoinInfo;
	vector<OBJINFO>	m_vecPageInfo;
	vector<OBJINFO>	m_vecMonsterInfo;

public:
	virtual void Free() override;
};

END