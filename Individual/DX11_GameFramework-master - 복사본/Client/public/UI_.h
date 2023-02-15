#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_ : public CGameObject
{
protected:
	CUI_(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_(const CUI_& rhs);
	virtual ~CUI_() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_double TimeDelta) override;
	virtual void			Late_Tick(_double TimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	virtual HRESULT		SetUp_Components() { return S_OK; }
	virtual HRESULT		SetUp_ShaderResources() { return S_OK; }

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;	

protected:
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

	_float					m_fX = 0.0f;
	_float					m_fY = 0.0f;
	_float					m_fSizeX = 0.0f;
	_float					m_fSizeY = 0.0f;

private:
	vector<CUI_*>			m_vecUI;

public:
	static CUI_*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END