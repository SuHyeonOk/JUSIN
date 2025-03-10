#include "stdafx.h"
#include "..\public\Sky.h"
#include "GameInstance.h"

#include "Obj_Manager.h"

CSky::CSky(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{

}

HRESULT CSky::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CSky::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;		

	return S_OK;
}

void CSky::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
}

void CSky::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&pGameInstance->Get_CamPosition()));

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();
	
	return S_OK;
}

HRESULT CSky::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxCubeTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level() || LEVEL_MINIGAME == CObj_Manager::GetInstance()->Get_Current_Level())
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_Garden"), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (LEVEL_SKELETON == CObj_Manager::GetInstance()->Get_Current_Level())
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Texture_Sky_Garden_Night"), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (LEVEL_SKELETON_BOSS == CObj_Manager::GetInstance()->Get_Current_Level())
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Texture_Sky_Dead"), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	



	return S_OK;
}

CSky * CSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSky*		pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSky::Clone(void * pArg)
{
	CSky*		pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
