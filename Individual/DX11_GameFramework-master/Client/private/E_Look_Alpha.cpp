#include "stdafx.h"
#include "..\public\E_Look_Alpha.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CE_Look_Alpha::CE_Look_Alpha(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Look_Alpha::CE_Look_Alpha(const CE_Look_Alpha & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Look_Alpha::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Look_Alpha::Initialize(void * pArg)
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

	_float fRandomSize = 0.0f;
	if (CE_Look_Alpha::EFFECTINFO::TEXTURETYPE::BIGSKELETON_TEXTURE == m_tEffectInfo.eTextureType)
		fRandomSize = 2.0f;
	else if (CE_Look_Alpha::EFFECTINFO::TEXTURETYPE::BIGSKELETON_TEXTURE == m_tEffectInfo.eTextureType)
		fRandomSize = CUtilities_Manager::GetInstance()->Get_Random(0.3f, 0.5f);

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(fRandomSize, fRandomSize, 1.f));

	// 알파 값
	m_fAlpha = 1.0f;
	
	return S_OK;
}

void CE_Look_Alpha::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 카메라를 바라보고 랜덤한 곳으로 회전하면서 이동하기
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);

	m_bStopAlpha_TimeAcc += TimeDelta;
	if(2.0 < m_bStopAlpha_TimeAcc)
		m_fAlpha -= _float(TimeDelta) * 0.4f;

	if (0 >= m_fAlpha)
		CGameObject::Set_Dead();	// 알파값이 다 사라지면 죽음
}

void CE_Look_Alpha::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Look_Alpha::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Look_Alpha::SetUp_Components()
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

	if (CE_Look_Alpha::EFFECTINFO::TEXTURETYPE::BIGSKELETON_TEXTURE == m_tEffectInfo.eTextureType)
		wsprintf(m_szTextureName, TEXT("Prototype_Component_Texture_E_Boss_SkeletonBig"));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureName, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Look_Alpha::SetUp_ShaderResources()
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

CE_Look_Alpha * CE_Look_Alpha::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Look_Alpha*		pInstance = new CE_Look_Alpha(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Look_Alpha");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Look_Alpha::Clone(void * pArg)
{
	CE_Look_Alpha*		pInstance = new CE_Look_Alpha(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Look_Alpha");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Look_Alpha::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
