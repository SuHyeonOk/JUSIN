#include "stdafx.h"
#include "..\public\UI_MiniMap.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_MninMap::CUI_MninMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_MninMap::CUI_MninMap(const CUI_MninMap & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_MninMap::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CUI_MninMap::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 247.f / 1.4f;
	m_fSizeY = 306.f / 1.4f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(538.0f, 228.8f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_MninMap::Tick(_double TimeDelta)
{
	if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
		m_iMiMap_Texture = 0;
	if (LEVEL_SKELETON == CObj_Manager::GetInstance()->Get_Current_Level())
		m_iMiMap_Texture = 1;
	else if (LEVEL_SKELETON_BOSS == CObj_Manager::GetInstance()->Get_Current_Level())
		m_iMiMap_Texture = 1;
}

void CUI_MninMap::Late_Tick(_double TimeDelta)
{

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_MninMap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
		pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("Garden"), _float2(1147.f, 19.f), 0.f, _float2(0.4f, 0.37f), XMVectorSet(0.16f, 0.10f, 0.08f, 1.f));
	if (LEVEL_SKELETON == CObj_Manager::GetInstance()->Get_Current_Level())
		pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("Skeleton"), _float2(1141.f, 19.f), 0.f, _float2(0.4f, 0.37f), XMVectorSet(0.16f, 0.10f, 0.08f, 1.f));
	else if (LEVEL_SKELETON_BOSS == CObj_Manager::GetInstance()->Get_Current_Level())
		pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("Skeleton Boss"), _float2(1147.f, 19.f), 0.f, _float2(0.4f, 0.37f), XMVectorSet(0.16f, 0.10f, 0.08f, 1.f));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_MninMap::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mini_Map"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MninMap::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iMiMap_Texture)))
		return E_FAIL;

	return S_OK;
}

CUI_MninMap * CUI_MninMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_MninMap*		pInstance = new CUI_MninMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MninMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MninMap::Clone(void * pArg)
{
	CUI_MninMap*		pInstance = new CUI_MninMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MninMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MninMap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
