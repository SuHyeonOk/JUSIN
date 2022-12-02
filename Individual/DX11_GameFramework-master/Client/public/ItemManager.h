#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CItemManager : public CBase
{
	DECLARE_SINGLETON(CItemManager)

public:
	CItemManager();
	virtual ~CItemManager() = default;

public:
	HRESULT	RandomCoin_Clone(_float3 f3StartPos, _uint iBronzeCount, _uint iSilverCount, _uint iGoldCount);

private:
	const _tchar*	m_szObjName;
	wstring			m_wstObjName;

public:
	virtual void Free() override;	
};

END