#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// 큐브텍스쳐 출력 버퍼 컴포넌트
class ENGINE_DLL CCubeTex : public CVIBuffer
{
private:
	explicit CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeTex(const CCubeTex& rhs);
	virtual ~CCubeTex();

public:
	virtual HRESULT Ready_Buffer(void) override;
	virtual void Render_Buffer(void) override;

public:
	static CCubeTex*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent*	Clone(void);
	virtual void Free(void) override;
};

END