#include "stdafx.h"
#include "..\public\UI_Level_BarBack.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_Level_BarBack::CUI_Level_BarBack(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_Level_BarBack::CUI_Level_BarBack(const CUI_Level_BarBack & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_Level_BarBack::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Level_BarBack::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 270.f / 1.5f;
	m_fSizeY = 15.f / 1.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-455.f, 248.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_Level_BarBack::Tick(_double TimeDelta)
{

}

void CUI_Level_BarBack::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Level_BarBack::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Render();

	// Coin Count
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_int iCount = CObj_Manager::GetInstance()->Get_Current_Player().iCoin;
	_tchar szCount[10];
	_itow_s(iCount, szCount, sizeof(szCount));

	pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(182.f, 113.f), 0.f, _float2(0.33f, 0.3f), XMVectorSet(1.0f, 0.84f, 0.0f, 1.f));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Level_BarBack::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LEVELBarBack"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Level_BarBack::SetUp_ShaderResources()
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

	_float fHPGauge = CUI_Manager::GetInstance()->Get_LevelGauge_Player();
	if (FAILED(m_pShaderCom->Set_RawValue("g_fHPGauge", &fHPGauge, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CUI_Level_BarBack * CUI_Level_BarBack::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_Level_BarBack*		pInstance = new CUI_Level_BarBack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Level_BarBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Level_BarBack::Clone(void * pArg)
{
	CUI_Level_BarBack*		pInstance = new CUI_Level_BarBack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Level_BarBack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Level_BarBack::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
