#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_ abstract : public CGameObject
{
public:
	typedef struct tagUIDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float3			f3Pos;

	}UIDESC;

protected:
	CUI_(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_(const CUI_& rhs);
	virtual ~CUI_() = default;

public:		// 걍 필수
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(const _double& TimeDelta);
	virtual void		Late_Tick(const _double& TimeDelta);
	virtual HRESULT		Render();

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

protected:	// 자식에게 거의 필수적인 함수
	virtual HRESULT		SetUp_Components()		{ return S_OK; }
	virtual HRESULT		SetUp_ShaderResources() { return S_OK; }
	
protected:
	UIDESC				m_tUI_Desc;

protected:
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free() override;
};

END