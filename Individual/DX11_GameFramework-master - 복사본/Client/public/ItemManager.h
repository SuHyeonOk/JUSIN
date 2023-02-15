#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CItemManager final : public CBase
{
	DECLARE_SINGLETON(CItemManager)

public:
	CItemManager();
	virtual ~CItemManager() = default;

public:
	HRESULT	Food_Clone(_float3 f3StartPos);
	HRESULT	Key_Clone(_float3 f3StartPos);
	HRESULT	RandomPage_Clone(_float3 f3StartPos, _uint iCount = 1);
	HRESULT	RandomCoin_Clone(_float3 f3StartPos, _uint iBronzeCount, _uint iSilverCount, _uint iGoldCount);

private:
	const _tchar*	m_szObjName;
	wstring			m_wstObjName;

private:
	_double			m_Effect_TimeAcc = 0;

public:
	virtual void Free() override;	
};

END