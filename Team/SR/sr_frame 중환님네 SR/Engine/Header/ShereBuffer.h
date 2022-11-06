#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CShereBuffer : public CVIBuffer
{
private:
	explicit CShereBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShereBuffer(const CShereBuffer& rhs);
	virtual ~CShereBuffer() override;

public:
	HRESULT Ready_Buffer(_uint iStackCount, _uint iSliceCount);
	virtual void Render_Buffer() override;

	static CShereBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iStackCount, _uint iSliceCount);
	virtual CComponent* Clone() override;
	virtual void Free() override;

private:
	LPD3DXMESH				gpSphere = NULL;

};
END
