#include "stdafx.h"
#include "..\public\E_Alpha_Change.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CE_Alpha_Change::CE_Alpha_Change(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Alpha_Change::CE_Alpha_Change(const CE_Alpha_Change & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Alpha_Change::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Alpha_Change::Initialize(void * pArg)
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

	m_iTexture_Index = 0;
	m_dChange_Texture = 0;

	if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::COLOR_HIT_TEXTURE == m_tEffectInfo.eTextureType)
		m_pTransformCom->Set_Scaled(_float3(1.5f, 1.5f, 1.f));
	else if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::BURN_FIRE_TEXTURE == m_tEffectInfo.eTextureType || 
		CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::JAKESON_TRANSFORM_TEXTURE == m_tEffectInfo.eTextureType)
		m_pTransformCom->Set_Scaled(_float3(2.0f, 2.0f, 1.f));
	
	if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::BURN_FIRE_TEXTURE == m_tEffectInfo.eTextureType)
		m_iDead_Number = 10;
	else if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::JAKESON_TRANSFORM_TEXTURE == m_tEffectInfo.eTextureType)
		m_iDead_Number = 3;

	return S_OK;
}

void CE_Alpha_Change::Tick(_double TimeDelta)
{
	// 카메라를 바라보고, 텍스처를 변경한다.
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);

	m_dChange_Texture += TimeDelta;
	if (0.09 < m_dChange_Texture)				// 이미지가 변경되는 시간
	{
		++m_iTexture_Index;
		m_dChange_Texture = 0;
	}

	if (0 != m_iDead_Number)
	{
		if (4 <= m_iTexture_Index)
		{
			++m_iDead_Count;
			m_iTexture_Index = 0;
		}

		if (m_iDead_Number <= m_iDead_Count)
			CGameObject::Set_Dead();
	}
	else
	{
		if (4 <= m_iTexture_Index)					// 삭제될 때
			CGameObject::Set_Dead();
	}
}

void CE_Alpha_Change::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameObject::Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Alpha_Change::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// 색 조정
	if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::COLOR_HIT_TEXTURE == m_tEffectInfo.eTextureType)
		m_pShaderCom->Begin(4);
	// 이미지색
	else
		m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Alpha_Change::SetUp_Components()
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

	_tchar	m_szTextureName[MAX_PATH] = L"";

	if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::HIT_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::COLOR_HIT_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Hit_Cahange"));
	else if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::JAKESON_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::JAKESON_TRANSFORM_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Jake_Son"));
	else if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::BOOM_FIRE_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Boss_Boom_Fire"));
	else if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::BURN_FIRE_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Boss_Burn"));
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Alpha_Change::SetUp_ShaderResources()
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
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexture_Index)))
		return E_FAIL;

	_float fAlpha = 1.0f;
	if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::HIT_TEXTURE == m_tEffectInfo.eTextureType)
		fAlpha = 0.2f;
	else if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::COLOR_HIT_TEXTURE == m_tEffectInfo.eTextureType)
		fAlpha = 0.7f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
		return E_FAIL;

	if (CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::COLOR_HIT_TEXTURE == m_tEffectInfo.eTextureType)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &m_tEffectInfo.f3Color, sizeof _float3)))
			return E_FAIL;
	}

	return S_OK;
}

CE_Alpha_Change * CE_Alpha_Change::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Alpha_Change*		pInstance = new CE_Alpha_Change(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Alpha_Change");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Alpha_Change::Clone(void * pArg)
{
	CE_Alpha_Change*		pInstance = new CE_Alpha_Change(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Alpha_Change");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Alpha_Change::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
