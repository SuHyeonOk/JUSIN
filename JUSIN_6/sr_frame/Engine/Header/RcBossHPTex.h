#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CRcBossHPTex : public CVIBuffer
{
private:
	explicit CRcBossHPTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcBossHPTex(const CVIBuffer& rhs);
	virtual ~CRcBossHPTex() override;

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
		if (m_fProgress < 0.f || m_fProgress > 1.f)
			m_fProgress = 1;
		m_fProgress = fProgress;
	}
	void RendEnd() { m_bRend = false; }

public:
	static CRcBossHPTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone() override;
	virtual void Free() override;

private:
	_float m_fProgress = 1.f;
	_float m_fPreProfress = 1.f;
	_bool m_bRend = true;
};
END