#include "stdafx.h"
#include "..\public\B_3DBullet.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CB_3DBullet::CB_3DBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CB_3DBullet::CB_3DBullet(const CB_3DBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CB_3DBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_3DBullet::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tBulletInfo, pArg, sizeof(NONANIMBULLETINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	m_wsTag = L"3DBullet";
	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tBulletInfo.f3Start_Pos.x, m_tBulletInfo.f3Start_Pos.y, m_tBulletInfo.f3Start_Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)
		m_pTransformCom->Set_Scaled(_float3(0.5f, 0.5f, 0.5f));

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_SKELETON)
	{
		_vector vTargetPos = XMLoadFloat4(&_float4(m_tBulletInfo.f3Target_Pos.x, m_tBulletInfo.f3Target_Pos.y, m_tBulletInfo.f3Target_Pos.z, 1.f));
		m_pTransformCom->LookAt(vTargetPos);
	}

	return S_OK;
}

void CB_3DBullet::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_SKELETON)
	{
		m_pTransformCom->Go_Straight(TimeDelta, 7.f);

		//m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.f, 0.0f, 1.f), 0.f);
	}

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)	// �÷��̾ ���� ȸ���ϸ� ���ư��� �Ѿ�
		Magic_Tick(TimeDelta);
}

void CB_3DBullet::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)	
		Magic_LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_BULLET, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CB_3DBullet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����Ѵ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

void CB_3DBullet::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		CGameObject::Set_Dead();
		CObj_Manager::GetInstance()->Set_Player_MinusHp(m_tBulletInfo.iMonsterAttack);

		if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)
		{
			if(CObj_Manager::PLAYERINFO::MAGIC != CObj_Manager::GetInstance()->Get_Current_Player().eState)
				CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::MAGIC);	// �÷��̾� State �� �����Ѵ�.
		}
		else
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::HIT);
	}
}

HRESULT CB_3DBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC) // ���
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_FireDragon_Area_FX"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_SKELETON) // ���
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_B_Bone"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	
	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_MAGIC)	// ���
	{
		ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}
	else if (m_tBulletInfo.eBulletType == m_tBulletInfo.TYPE_SKELETON)	// ���
	{
		ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CB_3DBullet::SetUp_ShaderResources()
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
void CB_3DBullet::Magic_Tick(const _double & TimeDelta)
{
	// Look ���� �������� ȸ���Ѵ�.
	m_pTransformCom->Go_Straight(TimeDelta * 2);
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta * 2);
}

void CB_3DBullet::Magic_LateTick(const _double & TimeDelta)
{
	// 10 �ʰ� ������ �����Ѵ�.
	m_dBullet_TimeAcc += TimeDelta;
	if (10 < m_dBullet_TimeAcc)
	{
		CGameObject::Set_Dead();
		m_dBullet_TimeAcc = 0;
	}
}

CB_3DBullet * CB_3DBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CB_3DBullet*		pInstance = new CB_3DBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CB_3DBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CB_3DBullet::Clone(void * pArg)
{
	CB_3DBullet*		pInstance = new CB_3DBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CB_3DBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CB_3DBullet::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
