#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CRcTex : public CVIBuffer
{
private:
	explicit CRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcTex(const CVIBuffer& rhs);
	virtual ~CRcTex() override;

public:
	virtual HRESULT Ready_Buffer() override;
	virtual void Render_Buffer() override;
	void Progress(_float fValue) 
	{ 
		m_fProgress += fValue;
		if (m_fProgress < 0.f || m_fProgress > 1.f)
		{
			m_fProgress = m_fPreProfress;
		}
	}
	void SetProgress(_float fProgress)
	{
		if (fProgress < 0.f || fProgress > 1.f)
			return;
		m_fProgress = fProgress;
	}

public:
	static CRcTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone() override;
	virtual void Free() override;

private:
	_float m_fProgress = 1.f;
	_float m_fPreProfress = 1.f;
};
END