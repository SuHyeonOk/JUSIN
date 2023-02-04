#include "stdafx.h"
#include "..\public\E_NoLook_Alpha.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CE_NoLook_Alpha::CE_NoLook_Alpha(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_NoLook_Alpha::CE_NoLook_Alpha(const CE_NoLook_Alpha & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_NoLook_Alpha::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_NoLook_Alpha::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tEffectInfo, pArg, sizeof(m_tEffectInfo));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tEffectInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	// Right 을 기준으로 90도 랜덤으로 회전
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = m_pTransformCom->Get_Scaled();
	_float		fAngle = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 360.0f);
	_matrix		RotationMatrix = XMMatrixRotationAxis(vLook, XMConvertToRadians(fAngle));

	vRight = vRight * vScale.x;
	vUp = vUp * vScale.y;
	vLook = vLook * vScale.z;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(90.f));

	m_fAlpha = 1.0f;

	return S_OK;
}

void CE_NoLook_Alpha::Tick(_double TimeDelta)
{
	// 카메라를 바라보지 않고, 알파값이 줄어들면서 사라진다.

	__super::Tick(TimeDelta);

	m_fAlpha -= _float(TimeDelta) * 0.2f;

	if(0.0f > m_fAlpha)
		CGameObject::Set_Dead();
}

void CE_NoLook_Alpha::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameObject::Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_NoLook_Alpha::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_NoLook_Alpha::SetUp_Components()
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

	if (CE_NoLook_Alpha::EFFECTINFO::TEXTURETYPE::TREEBULLET_TEXTURE == m_tEffectInfo.eTextureType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Witch"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CE_NoLook_Alpha::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_NoLook_Alpha * CE_NoLook_Alpha::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_NoLook_Alpha*		pInstance = new CE_NoLook_Alpha(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_NoLook_Alpha");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_NoLook_Alpha::Clone(void * pArg)
{
	CE_NoLook_Alpha*		pInstance = new CE_NoLook_Alpha(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_NoLook_Alpha");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_NoLook_Alpha::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
