#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTex : 	public CVIBuffer
{
private:
	explicit CRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex();

public:
	virtual HRESULT Ready_Buffer(void) override;
	virtual void Render_Buffer(void) override;

public:
	static CRcTex*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent*	Clone(void);
	virtual void Free(void) override;
};

END