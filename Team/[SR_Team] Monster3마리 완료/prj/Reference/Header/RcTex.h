#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// 텍스쳐를 "출력"할 버퍼 생성
// CTexture 컴포넌트는 텍스처 컴객체를 생성해서 보관하는 역할이다.
// 색 출력과 텍스쳐 출력은 별개(동시출력x)
// 정점을 세팅해 그 UV 좌표를 기준으로 출력할 버퍼가 필요하다.
class ENGINE_DLL CRcTex : public CVIBuffer
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
	CComponent*		Clone(void);
	virtual void Free(void) override;
};

END


