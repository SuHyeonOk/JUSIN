#include "stdafx.h"
#include "..\public\E_Skill_Marceline_Sound.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "PipeLine.h"
#include "Utilities_Manager.h"

CE_Skill_Marceline_Sound::CE_Skill_Marceline_Sound(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Skill_Marceline_Sound::CE_Skill_Marceline_Sound(const CE_Skill_Marceline_Sound & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Skill_Marceline_Sound::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Skill_Marceline_Sound::Initialize(void * pArg)
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

	return S_OK;
}

void CE_Skill_Marceline_Sound::Tick(_double TimeDelta)
{
	// 사이즈가 커지면서, 일정 사이즈가 되고 알파값이 줄어들면 사라진다.

	__super::Tick(TimeDelta);

	if (0.5f > m_fSizeX)
	{
		m_fSizeX += _float(TimeDelta) * 0.5f;
		m_fSizeY += _float(TimeDelta) * 0.5f;
	}
	else
		m_fAlpha -= _float(TimeDelta);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (0 >= m_fAlpha)
		CGameObject::Set_Dead();	// 알파값이 다 사라지면 죽음
}

void CE_Skill_Marceline_Sound::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Skill_Marceline_Sound::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Skill_Marceline_Sound::SetUp_Components()
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

	if (EFFECTINFO::EFFECTTYPE::SOUND == m_tEffectInfo.eEffectType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Skill_Marceline_Sound"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (EFFECTINFO::EFFECTTYPE::HP == m_tEffectInfo.eEffectType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Food_Hp"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (EFFECTINFO::EFFECTTYPE::INK == m_tEffectInfo.eEffectType)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Change_ColorSmoke"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CE_Skill_Marceline_Sound::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Skill_Marceline_Sound * CE_Skill_Marceline_Sound::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Skill_Marceline_Sound*		pInstance = new CE_Skill_Marceline_Sound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Skill_Marceline_Sound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Skill_Marceline_Sound::Clone(void * pArg)
{
	CE_Skill_Marceline_Sound*		pInstance = new CE_Skill_Marceline_Sound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Skill_Marceline_Sound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Skill_Marceline_Sound::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
