#include "stdafx.h"
#include "..\public\E_NoLook_Grow.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CE_NoLook_Grow::CE_NoLook_Grow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_NoLook_Grow::CE_NoLook_Grow(const CE_NoLook_Grow & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_NoLook_Grow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_NoLook_Grow::Initialize(void * pArg)
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
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(90.f));

	m_fAlpha = 1.0f;
	m_fSizeX = 0.0f;
	m_fSizeY = 0.0f; 
	
	return S_OK;
}

void CE_NoLook_Grow::Tick(_double TimeDelta)
{
	// 이미지를 원하는 색상으로 지정하고, x,z 로 사이즈가 커지다가 사라진다. (카메라를 바라보지 않는다.)

	__super::Tick(TimeDelta);

	m_fSizeX += _float(TimeDelta) * 3.f;
	m_fSizeY += _float(TimeDelta) * 3.f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (CE_NoLook_Grow::EFFECTINFO::TEXTURETYPE::MARVELINE_TEXTURE == m_tEffectInfo.eTextureType)
	{
		if (5.f < m_fSizeX)
			m_fAlpha -= _float(TimeDelta) * 0.3f;
	}

	if (8.f < m_fSizeX)
		CGameObject::Set_Dead();
}

void CE_NoLook_Grow::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_NoLook_Grow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if(CE_NoLook_Grow::EFFECTINFO::TEXTURETYPE::MARVELINE_TEXTURE == m_tEffectInfo.eTextureType)
		m_pShaderCom->Begin(4);		// 색상 지정하고, 알파값 들어가긴 하는데 텍스처 적용 안 되는
	else
		m_pShaderCom->Begin(2);		// 텍스처 그대로 사용하고, 알파값을 줄인다.

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_NoLook_Grow::SetUp_Components()
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

	if (CE_NoLook_Grow::EFFECTINFO::TEXTURETYPE::MARVELINE_TEXTURE == m_tEffectInfo.eTextureType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Skill_Marceline_Waves"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (CE_NoLook_Grow::EFFECTINFO::TEXTURETYPE::SWIM_TEXTURE == m_tEffectInfo.eTextureType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Swim"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CE_NoLook_Grow::SetUp_ShaderResources()
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

	if (CE_NoLook_Grow::EFFECTINFO::TEXTURETYPE::MARVELINE_TEXTURE == m_tEffectInfo.eTextureType)
	{
		_float3 fColor = { 0.85f, 0.90f, 0.49f };
		if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &fColor, sizeof _float3)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_NoLook_Grow * CE_NoLook_Grow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_NoLook_Grow*		pInstance = new CE_NoLook_Grow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_NoLook_Grow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_NoLook_Grow::Clone(void * pArg)
{
	CE_NoLook_Grow*		pInstance = new CE_NoLook_Grow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_NoLook_Grow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_NoLook_Grow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
