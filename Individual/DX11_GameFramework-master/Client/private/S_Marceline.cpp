#include "stdafx.h"
#include "..\public\S_Marceline.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CS_Marceline::CS_Marceline(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Marceline::CS_Marceline(const CS_Marceline & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Marceline::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Marceline::Initialize(void * pArg)
{	
	m_wsTag = L"Skill_Marceline";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	m_eState = DOWN;

	return S_OK;
}

void CS_Marceline::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Animation_Tick();
	State_Tick();
}

void CS_Marceline::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CS_Marceline::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

HRESULT CS_Marceline::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Marceline"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(7.0f, 7.0f, 7.0f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Marceline::SetUp_ShaderResources()
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

	return S_OK;
}

void CS_Marceline::Animation_Tick()
{
	switch (m_eState)
	{
	case Client::CS_Marceline::DOWN:
		m_pModelCom->Set_AnimIndex(2, false, false);
		break;
	case Client::CS_Marceline::SONG:
		m_pModelCom->Set_AnimIndex(1, false, false);
		break;
	case Client::CS_Marceline::UP:
		m_pModelCom->Set_AnimIndex(0, false, false);
		break;
	}
}

void CS_Marceline::State_Tick()
{
	switch (m_eState)
	{
	case Client::CS_Marceline::DOWN:
	{
		if (2 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
			m_eState = SONG;
	}
		break;
	case Client::CS_Marceline::SONG:
	{
		if (1 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		{
			m_eAnim_Count++;	// 애니메이션 2 번 재생하기 위해서

			if (2 <= m_eAnim_Count)
			{
				m_eState = UP;
				m_eAnim_Count = 0;
			}
		}
	}
		break;
	case Client::CS_Marceline::UP:
	{
		if (0 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
			CGameObject::Set_Dead();
	}
		break;
	}
}

CS_Marceline * CS_Marceline::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Marceline*		pInstance = new CS_Marceline(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Marceline");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Marceline::Clone(void * pArg)
{
	CS_Marceline*		pInstance = new CS_Marceline(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Marceline");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Marceline::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
