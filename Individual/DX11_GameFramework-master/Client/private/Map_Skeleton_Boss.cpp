#include "stdafx.h"
#include "..\public\Map_Skeleton_Boss.h"
#include "GameInstance.h"

CMap_Skleton_Boss::CMap_Skleton_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CMap_Skleton_Boss::CMap_Skleton_Boss(const CMap_Skleton_Boss & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMap_Skleton_Boss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Skleton_Boss::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Play_Sound(TEXT("Boss1_Loop.ogg"), 0.5f, true);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMap_Skleton_Boss::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMap_Skleton_Boss::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MAP_NONALPHABLEND, this);
}

HRESULT CMap_Skleton_Boss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 3);
	}

	return S_OK;
}

HRESULT CMap_Skleton_Boss::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_Skeleton_Boss"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Skleton_Boss::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMap_Skleton_Boss * CMap_Skleton_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMap_Skleton_Boss*		pInstance = new CMap_Skleton_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMap_Skleton_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMap_Skleton_Boss::Clone(void * pArg)
{
	CMap_Skleton_Boss*		pInstance = new CMap_Skleton_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMap_Skleton_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap_Skleton_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
