#include "stdafx.h"
#include "..\public\UI_Monster_Bar.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_Monstser_Bar::CUI_Monstser_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_Monstser_Bar::CUI_Monstser_Bar(const CUI_Monstser_Bar & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_Monstser_Bar::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Monstser_Bar::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 176.f / 1.5f;
	m_fSizeY = 14.f / 1.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(22.f, 280.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_Monstser_Bar::Tick(_double TimeDelta)
{

}

void CUI_Monstser_Bar::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Monstser_Bar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Monstser_Bar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Monster_HealthBar_Bar"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Monstser_Bar::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_float fHPGauge = CUI_Manager::GetInstance()->Get_HPGauge_Monster();
	if (FAILED(m_pShaderCom->Set_RawValue("g_fHPGauge", &fHPGauge, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CUI_Monstser_Bar * CUI_Monstser_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_Monstser_Bar*		pInstance = new CUI_Monstser_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Monstser_Bar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Monstser_Bar::Clone(void * pArg)
{
	CUI_Monstser_Bar*		pInstance = new CUI_Monstser_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Monstser_Bar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Monstser_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
