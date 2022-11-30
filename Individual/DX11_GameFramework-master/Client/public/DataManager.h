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

	void			Set_FoodInfo(const _tchar& ObjName, const _float3& ObjPos);
	void			Set_CoinInfo(const _tchar& ObjName, const _float3& ObjPos);

private:
	vector<OBJINFO>	m_vecFoodInfo;
	vector<OBJINFO>	m_vecCoinInfo;

public:
	virtual void Free() override;
};

END