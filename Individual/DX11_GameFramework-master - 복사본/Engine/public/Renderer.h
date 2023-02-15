#pragma once

#include "Component.h"

/* ȭ�鿡 �׷����� ��ü���� �׸��� ������� ���������Ѵ�. */
/* �����ϰ� �ִ� ������� ��ü���� ��ο���(�����Լ��� ȣ���Ѵ�)�� �����Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { 
		RENDER_PRIORITY,
		RENDER_SHADOWDEPTH,
		RENDER_MAP_NONALPHABLEND, 
		RENDER_NONALPHABLEND,
		RENDER_XRAYBLEND,
		RENDER_NONLIGHT, 
		RENDER_ALPHABLEND, 
		RENDER_UI, 
		RENDER_END
	};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_DebugRenderGroup(class CComponent* pComponent);
	HRESULT Draw_RenderGroup();

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

private:
	list<class CComponent*>				m_DebugObject;
	typedef list<class CComponent*>		DEBUGOBJECTS;

private:
	class CTarget_Manager*				m_pTarget_Manager = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	class CShader*						m_pShader = nullptr;
	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowDepth();
	HRESULT Render_Map_NonAlphaBlend();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_XRayBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();

private:
	HRESULT Render_DebugObject();

public:
	typedef struct tagLightDesc
	{
		_float fFar = 300.0f;
		_float fTemp = { 0.1f };
		_float4 f4LightEye = _float4(-5.0f, 15.0f, -5.0f, 1.0f);
		_float4 f4LightAt = { 60.0f, 0.0f, 60.0f, 1.0f };
		_float4 f4LightUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	}LIGHTDESC;

	void Set_LightDesc(LIGHTDESC * LightDesc)
	{
		memcpy(&m_LightDesc, LightDesc, sizeof(LIGHTDESC));
	}
	LIGHTDESC Get_LightDesc()
	{
		return m_LightDesc;
	}

private:
	LIGHTDESC m_LightDesc;

public:
	static CRenderer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRenderer*	Clone(void* pArg = nullptr) override;
	virtual void		Free() override;

};

END