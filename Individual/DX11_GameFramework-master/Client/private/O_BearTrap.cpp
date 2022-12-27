#include "stdafx.h"
#include "..\public\O_BearTrap.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CO_BearTrap::CO_BearTrap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CO_BearTrap::CO_BearTrap(const CO_BearTrap & rhs)
	: CGameObject(rhs)
{

}

HRESULT CO_BearTrap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CO_BearTrap::Initialize(void * pArg)
{	
	m_wsTag = L"Object_BeapTrap";

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

	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

void CO_BearTrap::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(1 == m_pModelCom->Get_AnimIndex(), m_pModelCom->Get_Finished())
		m_pModelCom->Set_AnimIndex(0);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_OBJ, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CO_BearTrap::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CO_BearTrap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (1 == i)
			continue;

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

void CO_BearTrap::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		m_pModelCom->Set_AnimIndex(1, false);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::HIT);
	}
}

HRESULT CO_BearTrap::SetUp_Components()
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
	if (FAILED(__super::Add_Component(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Prototype_Component_Model_O_BearTrap"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CO_BearTrap::SetUp_ShaderResources()
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

CO_BearTrap * CO_BearTrap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CO_BearTrap*		pInstance = new CO_BearTrap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CO_BearTrap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CO_BearTrap::Clone(void * pArg)
{
	CO_BearTrap*		pInstance = new CO_BearTrap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CO_BearTrap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CO_BearTrap::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
