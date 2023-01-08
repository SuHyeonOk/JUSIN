#include "stdafx.h"
#include "..\public\E_Food_Up.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CE_Food_Up::CE_Food_Up(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Food_Up::CE_Food_Up(const CE_Food_Up & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Food_Up::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Food_Up::Initialize(void * pArg)
{	
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(3.f, 3.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(90.f));

	return S_OK;
}

void CE_Food_Up::Tick(_double TimeDelta)
{
	// 이미지 색상 그대로 위로 올라간다.	(카메라를 바라보지 않는다.)

	__super::Tick(TimeDelta);

	m_pTransformCom->Go_Backward(TimeDelta);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vf4MyPos;
	XMStoreFloat4(&vf4MyPos, vMyPos);

	if (1.2f < vf4MyPos.y)
		CGameObject::Set_Dead();
}

void CE_Food_Up::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Food_Up::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Food_Up::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Food_Up"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Food_Up::SetUp_ShaderResources()
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

	// 텍스처 넘기기
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	_float3 fOutColor = { 0.0f, 0.0f, 0.0f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_fOutColor", &fOutColor, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Food_Up * CE_Food_Up::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Food_Up*		pInstance = new CE_Food_Up(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Food_Up");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Food_Up::Clone(void * pArg)
{
	CE_Food_Up*		pInstance = new CE_Food_Up(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Food_Up");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Food_Up::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
