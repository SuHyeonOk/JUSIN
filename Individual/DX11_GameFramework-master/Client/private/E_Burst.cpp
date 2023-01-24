#include "stdafx.h"
#include "..\public\E_Burst.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "PipeLine.h"
#include "Utilities_Manager.h"

CE_Burst::CE_Burst(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Burst::CE_Burst(const CE_Burst & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Burst::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Burst::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_tEffectInfo, pArg, sizeof(EFFECTINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	//if(CE_Burst::EFFECTINFO::TEXTURETYPE::STAR_TEXTURE == m_tEffectInfo.eTextureType ||
	//	CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)
	//	GameObjectDesc.TransformDesc.fSpeedPerSec = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 2.0f);
	//else
	//	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tEffectInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_Pos();
	
	// 크기
	if (CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE == m_tEffectInfo.eTextureType ||
		CE_Burst::EFFECTINFO::TEXTURETYPE::STAR_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)			// 랜덤한  크기를 가지는 이미지
	{
		_float fRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(0.3f, 1.0f);

		m_pTransformCom->Set_Scaled(_float3(fRandomNumber, fRandomNumber, 1.f));
	}
	else																						// 고정된 크기를 가지는 이미지
	{
		m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 1.f));
	}

	// 알파값
	if (CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE == m_tEffectInfo.eTextureType)
		m_fAlpha = 0.5f;
	else
		m_fAlpha = 1.0f;

	// 속도
	if (CE_Burst::EFFECTINFO::TEXTURETYPE::STAR_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)
		m_fSpeed = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 0.3f);
	else if (CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE == m_tEffectInfo.eTextureType)
		m_fSpeed = 0.3f;
	else
		m_fSpeed = 0.2f;

	return S_OK;
}

void CE_Burst::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// ★ 카메라를 바라보고 랜덤한 곳으로 회전하면서 이동하기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);

	// 입력한 Look 방향으로 이동하기
	_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = XMLoadFloat4(&_float4(m_tEffectInfo.f4Look.x, m_tEffectInfo.f4Look.y, m_tEffectInfo.f4Look.z, 0.0f));
	vMyPos += XMVector3Normalize(vDistance) * _float(TimeDelta) * m_fSpeed;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);

	// ★ 알파값 줄어들기
	if (CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE == m_tEffectInfo.eTextureType)	// 그냥 바로 알파값 줄어든다.
	{
		m_fAlpha -= _float(TimeDelta) * 0.2f;
	}
	else																				// 일정시간 있다가 알파값 줄어든다.
	{
		m_dNoAlpha_TimeAcc += TimeDelta;
		if (1 < m_dNoAlpha_TimeAcc)
		{
			m_fAlpha -= _float(TimeDelta) * 0.5f;
			//m_dNoAlpha_TimeAcc = 0; // 어짜피 죽기 때문에.. '-'
		}
	}

	if (0 >= m_fAlpha)
		CGameObject::Set_Dead();	// 알파값이 다 사라지면 죽음
}

void CE_Burst::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameObject::Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Burst::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// 색 조정
	if (CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE == m_tEffectInfo.eTextureType ||
		CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)
		m_pShaderCom->Begin(4);
	// 이미지색
	else
		m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Burst::SetUp_Components()
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

	if (CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Change_Smoke"));
	else if (CE_Burst::EFFECTINFO::TEXTURETYPE::STAR_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Star"));
	else if (CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Star3"));
	else if (CE_Burst::EFFECTINFO::TEXTURETYPE::HEART_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Heart"));
	else if (CE_Burst::EFFECTINFO::TEXTURETYPE::FLOWER_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Flower"));
	else if (CE_Burst::EFFECTINFO::TEXTURETYPE::LEAF_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Leaf"));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Burst::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &m_tEffectInfo.f3Color, sizeof _float3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Burst * CE_Burst::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Burst*		pInstance = new CE_Burst(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Burst");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Burst::Clone(void * pArg)
{
	CE_Burst*		pInstance = new CE_Burst(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Burst");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Burst::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
