#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CDataManager : public CBase
{
	DECLARE_SINGLETON(CDataManager)

	typedef struct tagObjectInfo
	{
		_tchar		ObjName[MAX_PATH];
		_float3		ObjPos;

	}OBJINFO;

public:
	CDataManager();
	virtual ~CDataManager() = default;

public:
	vector<OBJINFO>	Get_FoodInfo() { return m_vecFoodInfo; }

	void			Set_FoodInfo(const _tchar& ObjName, const _float3& ObjPos, void* pArg = nullptr);

private:
	vector<OBJINFO>	m_vecFoodInfo;

public:
	virtual void Free() override;
};

END