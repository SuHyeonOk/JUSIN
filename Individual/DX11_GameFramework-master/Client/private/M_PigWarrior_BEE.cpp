#include "stdafx.h"
#include "..\public\M_PigWarrior_BEE.h"

#include "GameInstance.h"

CM_PigWarrior_BEE::CM_PigWarrior_BEE(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CM_PigWarrior_BEE::CM_PigWarrior_BEE(const CM_PigWarrior_BEE & rhs)
	: CGameObject(rhs)
{

}

HRESULT CM_PigWarrior_BEE::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior_BEE::Initialize(void * pArg)
{	
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (nullptr != pArg)
		memcpy(&m_tinMonsterInfo, pArg, sizeof(MONSTERINFO));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinMonsterInfo.fPos.x, m_tinMonsterInfo.fPos.y, m_tinMonsterInfo.fPos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CM_PigWarrior_BEE::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CM_PigWarrior_BEE::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_PigWarrior_BEE::Render()
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

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CM_PigWarrior_BEE::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_BEE"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior_BEE::SetUp_ShaderResources()
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

CM_PigWarrior_BEE * CM_PigWarrior_BEE::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_PigWarrior_BEE*		pInstance = new CM_PigWarrior_BEE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_PigWarrior_BEE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_PigWarrior_BEE::Clone(void * pArg)
{
	CM_PigWarrior_BEE*		pInstance = new CM_PigWarrior_BEE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_PigWarrior_BEE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_PigWarrior_BEE::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
