#include "stdafx.h"
#include "..\public\Map_MiniGame.h"
#include "GameInstance.h"
#include "Obj_Manager.h"

#include "Imgui_PropertyEditor.h"	// @

CMap_MiniGame::CMap_MiniGame(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CMap_MiniGame::CMap_MiniGame(const CMap_MiniGame & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMap_MiniGame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_MiniGame::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Stop_Sound(0);
	pGameInstance->Play_Sound(TEXT("Garden4_Loop.ogg"), 0.2f, true, 0);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMap_MiniGame::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	static _float					fShaderVealue[2] = { 0.0f };
	static _float					arrLightEye[4] = { 0.0f };
	static _float					arrLightAt[4] = { 0.0f };
	static _float					arrLightUp[4] = { 0.0f };

	//ImGui::SliderFloat("Far", &fShaderVealue[0], -50.0f, 50.0f);
	ImGui::InputFloat("Far", &fShaderVealue[0]);
	ImGui::InputFloat("0.1f", &fShaderVealue[1]);
	ImGui::InputFloat4("LightEye", arrLightEye);
	ImGui::InputFloat4("LightAt", arrLightAt);
	ImGui::InputFloat4("LightUp", arrLightUp);

	if (ImGui::Button("Set Light"))
	{
		CRenderer::LIGHTDESC	LightDesc;

		LightDesc.fFar = fShaderVealue[0];
		LightDesc.fTemp = fShaderVealue[1];
		LightDesc.f4LightEye = _float4(arrLightEye[0], arrLightEye[1], arrLightEye[2], arrLightEye[3]);
		LightDesc.f4LightAt = _float4(arrLightAt[0], arrLightAt[1], arrLightAt[2], arrLightAt[3]);
		LightDesc.f4LightUp = _float4(arrLightUp[0], arrLightUp[1], arrLightUp[2], arrLightUp[3]);

		m_pRendererCom->Set_LightDesc(&LightDesc);
	}
}

void CMap_MiniGame::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MAP_NONALPHABLEND, this);
	}
}

HRESULT CMap_MiniGame::Render()
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

HRESULT CMap_MiniGame::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_MiniGame"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_MiniGame::SetUp_ShaderResources()
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

CMap_MiniGame * CMap_MiniGame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMap_MiniGame*		pInstance = new CMap_MiniGame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMap_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMap_MiniGame::Clone(void * pArg)
{
	CMap_MiniGame*		pInstance = new CMap_MiniGame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMap_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap_MiniGame::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
