#include "stdafx.h"
#include "..\public\UI_3DTexture.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CUI_3DTexture::CUI_3DTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CUI_3DTexture::CUI_3DTexture(const CUI_3DTexture & rhs)
	: CGameObject(rhs)
{

}

HRESULT CUI_3DTexture::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_3DTexture::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tTextureInfo, pArg, sizeof(TEXTUREINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tTextureInfo.f3Pos.x, m_tTextureInfo.f3Pos.y, m_tTextureInfo.f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(m_tTextureInfo.f2Size.x, m_tTextureInfo.f2Size.y, 1.f));

	return S_OK;
}

void CUI_3DTexture::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (m_tTextureInfo.eTextureType == m_tTextureInfo.TYPE_FIND)
		Find_Tick(TimeDelta);
}

void CUI_3DTexture::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CUI_3DTexture::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);	// pass

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_3DTexture::SetUp_Components()
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

	if (m_tTextureInfo.eTextureType == m_tTextureInfo.TYPE_FIND)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_FindEnemy_FX"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_tTextureInfo.eTextureType == m_tTextureInfo.TYPE_TALK)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LooseEnemy_FX"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_3DTexture::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

void CUI_3DTexture::Find_Tick(const _double & TimeDelta)
{
	// 일정 시간 있다가 사라진다.
	m_pTransformCom->Set_Pos(_float3(m_tTextureInfo.f3Pos.x, m_tTextureInfo.f3Pos.y, m_tTextureInfo.f3Pos.z));

	m_dTexture_TimeAcc += TimeDelta;
	if (0.5 < m_dTexture_TimeAcc)
	{
		CGameObject::Set_Dead();
		m_dTexture_TimeAcc = 0;
	}
}

CUI_3DTexture * CUI_3DTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_3DTexture*		pInstance = new CUI_3DTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_3DTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_3DTexture::Clone(void * pArg)
{
	CUI_3DTexture*		pInstance = new CUI_3DTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_3DTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_3DTexture::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
