#include "stdafx.h"
#include "..\public\E_Look_Grow.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CE_Look_Grow::CE_Look_Grow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Look_Grow::CE_Look_Grow(const CE_Look_Grow & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Look_Grow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Look_Grow::Initialize(void * pArg)
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

	// 알파 값
	m_fAlpha = 1.0f;

	// 크기 조정 하는
	if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::SOUND_TEXTURE == m_tEffectInfo.eTextureType || 
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::HP_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::INK_TEXTURE == m_tEffectInfo.eTextureType)
	{
		m_fSizeX = 0.01f;
		m_fSizeY = 0.01f;
	}
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::SMALL_FIRE_TEXTURE == m_tEffectInfo.eTextureType)
	{
		m_fSizeX = 0.3f;
		m_fSizeY = 0.3f;
	}
	// 크기 조정 안 하는
	else
	{
		m_fSizeX = 1.0f;
		m_fSizeY = 1.0f;
	}

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeX, 1.f));

	return S_OK;
}

void CE_Look_Grow::Tick(_double TimeDelta)
{
	// 이미지를 원하는 색상으로 지정 or 텍스처 사용, 카메라를 바라보면서, x,z 로 사이즈가 커지다가 사라진다.

	__super::Tick(TimeDelta);

	// 카메라를 바라보고 랜덤한 곳으로 회전하면서 이동하기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);		// 카메라를 바라본다.

													// 빠르게 커지는
	if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::PAINT_CIRCLE_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::INK_TEXTURE == m_tEffectInfo.eTextureType)
	{
		m_fSizeX += _float(TimeDelta) * 1.2f;
		m_fSizeY += _float(TimeDelta) * 1.2f;
	}
	// 천천히 커지는
	else
	{
		m_fSizeX += _float(TimeDelta) * 0.2f;
		m_fSizeY += _float(TimeDelta) * 0.2f;
	}

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeX, 1.f));


	// ★ 알파값 줄어들기
	// 일정 시간 있다가 줄어들기
	if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::SOUND_TEXTURE == m_tEffectInfo.eTextureType || 
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::HP_TEXTURE == m_tEffectInfo.eTextureType)
	{
		m_dNoAlpha_TimeAcc += TimeDelta;
		if (2 < m_dNoAlpha_TimeAcc)
			m_fAlpha -= _float(TimeDelta) * 0.5f;
	}
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::INK_TEXTURE == m_tEffectInfo.eTextureType)
	{
		m_dNoAlpha_TimeAcc += TimeDelta;
		if (0.5 < m_dNoAlpha_TimeAcc)
			m_fAlpha -= _float(TimeDelta) * 0.5f;
	}
	// 빠르게 줄어들기
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::SMALL_FIRE_TEXTURE == m_tEffectInfo.eTextureType)
	{
		m_fAlpha -= _float(TimeDelta);
	}
	// 바로 줄어들기
	else																				
	{
		m_fAlpha -= _float(TimeDelta) * 0.5f;
	}

	if (0 >= m_fAlpha)
		CGameObject::Set_Dead();	// 알파값이 다 사라지면 죽음
}

void CE_Look_Grow::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Look_Grow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// 색 조정
	if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::PAINT_FIRECRAKER_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::PAINT_CIRCLE_TEXTURE == m_tEffectInfo.eTextureType || 
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::INK_TEXTURE == m_tEffectInfo.eTextureType ||
		CE_Look_Grow::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)
		m_pShaderCom->Begin(4);
	// 이미지색
	else																
		m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Look_Grow::SetUp_Components()
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

	if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::SOUND_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Skill_Marceline_Sound"));
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::HP_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Food_Hp"));
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::INK_TEXTURE == m_tEffectInfo.eTextureType ||
			 CE_Look_Grow::EFFECTINFO::TEXTURETYPE::INK_RANDOM_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Change_ColorSmoke"));
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::PAINT_FIRECRAKER_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Paint_Firecracker"));
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::PAINT_CIRCLE_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Paint_Circle"));
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Star_Random"));
	else if (CE_Look_Grow::EFFECTINFO::TEXTURETYPE::SMALL_FIRE_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Boss_Fire"));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Look_Grow::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &m_tEffectInfo.f3Color, sizeof _float3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Look_Grow * CE_Look_Grow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Look_Grow*		pInstance = new CE_Look_Grow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Look_Grow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Look_Grow::Clone(void * pArg)
{
	CE_Look_Grow*		pInstance = new CE_Look_Grow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Look_Grow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Look_Grow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
