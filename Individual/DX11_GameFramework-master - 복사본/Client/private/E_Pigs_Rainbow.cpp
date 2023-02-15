#include "stdafx.h"
#include "..\public\E_Pigs_Rainbow.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CE_Pigs_Rainbow::CE_Pigs_Rainbow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Pigs_Rainbow::CE_Pigs_Rainbow(const CE_Pigs_Rainbow & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Pigs_Rainbow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Pigs_Rainbow::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tEffectInfo, pArg, sizeof(m_tEffectInfo));

	CGameObject::GAMEOBJECTDESC      GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tEffectInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(3.0f, 0.5f, 1.f));

	return S_OK;
}

void CE_Pigs_Rainbow::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// Set_Dead 하는 2가지 경우 1) 플레이어와 닿았을 때, 2) 일정시간이 지났을 때

	m_fSize_X += _float(TimeDelta);

	if(1.0f < m_fSize_X)
		CGameObject::Set_Dead();



	//m_fAlpha -= _float(TimeDelta) * 0.2f;

	//if (0.0f > m_fAlpha)
	//	CGameObject::Set_Dead();
}

void CE_Pigs_Rainbow::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameObject::Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Pigs_Rainbow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Pigs_Rainbow::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pigs_Rainbow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Pigs_Rainbow::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	// 텍스처 넘기기
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fHPGauge", &m_fSize_X, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Pigs_Rainbow * CE_Pigs_Rainbow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Pigs_Rainbow*      pInstance = new CE_Pigs_Rainbow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Pigs_Rainbow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Pigs_Rainbow::Clone(void * pArg)
{
	CE_Pigs_Rainbow*      pInstance = new CE_Pigs_Rainbow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Pigs_Rainbow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Pigs_Rainbow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}