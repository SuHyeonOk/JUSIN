#include "stdafx.h"
#include "..\public\E_Skill_Marceline_Waves.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CE_Skill_Marceline_Waves::CE_Skill_Marceline_Waves(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_Skill_Marceline_Waves::CE_Skill_Marceline_Waves(const CE_Skill_Marceline_Waves & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_Skill_Marceline_Waves::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Skill_Marceline_Waves::Initialize(void * pArg)
{	
	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMConvertToRadians(90.f));

	return S_OK;
}

void CE_Skill_Marceline_Waves::Tick(_double TimeDelta)
{
	// 이미지를 원하는 색상으로 지정하고, x,z 로 사이즈가 커지다가 사라진다. (카메라를 바라보지 않는다.)

	__super::Tick(TimeDelta);

	m_fSizeX += _float(TimeDelta) * 3.f;
	m_fSizeY += _float(TimeDelta) * 3.f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (5.f < m_fSizeX)
		m_fAlpha -= _float(TimeDelta) * 0.3f;

	if (8.f < m_fSizeX)
		CGameObject::Set_Dead();
}

void CE_Skill_Marceline_Waves::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_Skill_Marceline_Waves::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_Skill_Marceline_Waves::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Skill_Marceline_Waves"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Skill_Marceline_Waves::SetUp_ShaderResources()
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

	_float3 fColor = { 0.85f, 0.90f, 0.49f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_fColor", &fColor, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_Skill_Marceline_Waves * CE_Skill_Marceline_Waves::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Skill_Marceline_Waves*		pInstance = new CE_Skill_Marceline_Waves(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_Skill_Marceline_Waves");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Skill_Marceline_Waves::Clone(void * pArg)
{
	CE_Skill_Marceline_Waves*		pInstance = new CE_Skill_Marceline_Waves(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_Skill_Marceline_Waves");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Skill_Marceline_Waves::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
