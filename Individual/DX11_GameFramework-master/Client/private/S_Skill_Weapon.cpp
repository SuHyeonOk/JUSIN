#include "stdafx.h"
#include "..\public\S_Skill_Weapon.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CS_Skill_Weapon::CS_Skill_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Skill_Weapon::CS_Skill_Weapon(const CS_Skill_Weapon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Skill_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Skill_Weapon::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (WEAPONDESC::WEAPON::JAKE_MAGIC == m_WeaponDesc.eWeaponType)
		m_wsTag = L"Jake_Magic_Weapon";	// 일반 몬스터는 공격 받지 않고, 매직 몬스터만 공격 받기 때문에
	else
		m_wsTag = L"Player_Weapon";

	return S_OK;
}

void CS_Skill_Weapon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (CSkill_Manager::MAGICSKILL::IDLE == CSkill_Manager::GetInstance()->Get_Magic_Skill().eSkill ||
		CSkill_Manager::FIONASKILL::IDLE == CSkill_Manager::GetInstance()->Get_Fiona_Skill().eSkill)
		CUI_Manager::GetInstance()->Set_Ui_Monster(false);	// 공격이 아닌 상태에서는 몬스터 UI 를 끈다.
}

void CS_Skill_Weapon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	_matrix			SocketMatrix = m_WeaponDesc.pSocket->Get_OffsetMatrix() *
		m_WeaponDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = SocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT   CS_Skill_Weapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CS_Skill_Weapon::On_Collision(CGameObject * pOther)
{
	if (L"Not_UI" != pOther->Get_Tag())
	{
		CUI_Manager::GetInstance()->Set_Ui_Monster(true);
		CUI_Manager::GetInstance()->UI_Monster_Index(pOther);
	}
}

HRESULT CS_Skill_Weapon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC			ColliderDesc;

	if (WEAPONDESC::WEAPON::JAKE_MAGIC == m_WeaponDesc.eWeaponType)
	{
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(1.0f, 1.0f, 1.0f);
		ColliderDesc.vCenter = _float3(0.0f, -1.5f, 0.0f);
	}
	else if (WEAPONDESC::WEAPON::FIONA_SWORD == m_WeaponDesc.eWeaponType)
	{
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
		ColliderDesc.vCenter = _float3(0.6f, 0.0f, -0.15f);
	}
	else if (WEAPONDESC::WEAPON::FIONA_CAT == m_WeaponDesc.eWeaponType)
	{
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
		ColliderDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Skill_Weapon::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CS_Skill_Weapon * CS_Skill_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Skill_Weapon*		pInstance = new CS_Skill_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Skill_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Skill_Weapon::Clone(void * pArg)
{
	CS_Skill_Weapon*		pInstance = new CS_Skill_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Skill_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Skill_Weapon::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_WeaponDesc.pSocket);
		Safe_Release(m_WeaponDesc.pTargetTransform);
	}

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
