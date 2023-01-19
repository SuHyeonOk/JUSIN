#include "stdafx.h"
#include "..\public\Jake_Weapon.h"

#include "GameInstance.h"
#include "Bone.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

CJake_Weapon::CJake_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CJake_Weapon::CJake_Weapon(const CJake_Weapon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CJake_Weapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CJake_Weapon::Initialize(void * pArg)
{
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon ||
		CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
		m_wsTag = L"Player_Weapon";
	else if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
		m_wsTag = L"Jake_Shield";

	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CJake_Weapon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 현재 플레이어가 무기의 주인일 때
	if (CObj_Manager::PLAYERINFO::PLAYER::JAKE == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		if (CObj_Manager::PLAYERINFO::IDLE == CObj_Manager::GetInstance()->Get_Current_Player().eState)
			CUI_Manager::GetInstance()->Set_Ui_Monster(false);
	}

	// 쉴드 객체의 경우 에만 다음을 실행한다.
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD == m_WeaponDesc.eWeaponType)	
	{
		if (true == CObj_Manager::GetInstance()->Get_Jake_Shield())	// 제이크 쉴드가 충돌 했을 때! 쉴드 이펙트를 호출하면 계속 생성되기 때문에
		{
			if (!m_bEffect_Shield)
			{
				_vector vMyPos = m_WeaponDesc.pTargetTransform->Get_State(CTransform::STATE_TRANSLATION);
				_float4 f4MyPos;
				XMStoreFloat4(&f4MyPos, vMyPos);

				CEffect_Manager::GetInstance()->Effect_Shield_Create(_float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z - 0.5f));
				m_bEffect_Shield = true;
			}
		}
	}

}

void CJake_Weapon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	_matrix			SocketMatrix = m_WeaponDesc.pSocket->Get_OffsetMatrix() *
		m_WeaponDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = SocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	if (CObj_Manager::PLAYERINFO::PLAYER::JAKE == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix() * SocketMatrix);
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT   CJake_Weapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
	{
		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, i, nullptr, 2);
		}
	}

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

void CJake_Weapon::On_Collision(CGameObject * pOther)
{
	// 내가 플레이어 일 때만 체크할거야
	if (CObj_Manager::PLAYERINFO::JAKE != CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		return;

	if (L"Gary_Boss" != pOther->Get_Tag())
		CUI_Manager::GetInstance()->Set_Ui_Monster(true);		// 나 지금 몬스터랑 충돌 해서 UI 를 띄울게
	CUI_Manager::GetInstance()->UI_Monster_Index(pOther);	// 충돌한 몬스터는 이거야

	// 내가 제이크 쉴드야
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD == m_WeaponDesc.eWeaponType)
		CObj_Manager::GetInstance()->Set_Jake_Shield(true);	// 나 충돌했어

}

HRESULT CJake_Weapon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	if (m_WeaponDesc.eWeaponType == CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST)
	{
		/* For.Com_SPHERE */
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}
	else if (m_WeaponDesc.eWeaponType == CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST)
	{
		/* For.Com_SPHERE */
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}
	else if (m_WeaponDesc.eWeaponType == CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_W_Jake_Punch_Shield"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_SPHERE */
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJake_Weapon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// g_WorldMatrix * SocketMatrix;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;


	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CJake_Weapon * CJake_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CJake_Weapon*		pInstance = new CJake_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CJake_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJake_Weapon::Clone(void * pArg)
{
	CJake_Weapon*		pInstance = new CJake_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CJake_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJake_Weapon::Free()
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
