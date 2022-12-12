#include "stdafx.h"
#include "..\public\M_Tree_Witch.h"

#include "GameInstance.h"
#include "Obj_Manager.h"		// �÷��̾� ���� ��������..
#include "ItemManager.h"		// ���� �� ���� ��Ʈ������..
#include "Skill_Manager.h"		// ��ų �÷��̾�� �����Ϸ���..
#include "Utilities_Manager.h"	// ������ ������..

CM_Tree_Wolf::CM_Tree_Wolf(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Tree_Wolf::CM_Tree_Wolf(const CM_Tree_Wolf & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Tree_Wolf::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Tree_Wolf::Initialize(void * pArg)
{
	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	MonsterDesc.TransformDesc.fSpeedPerSec		= 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec	= XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos				= _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.IDLE;
	m_tMonsterInfo.iHp		= 70;
	m_tMonsterInfo.iExp		= 50;
	m_tMonsterInfo.iAttack	= 15;

	return S_OK;
}

void CM_Tree_Wolf::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		// TODO : �浹ó���� ������ ���� ���� (�� �� �� ���� ȣ�� �Ǿ�� ��)
		m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
		m_tMonsterInfo.eState = m_tMonsterInfo.HIT;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CM_Tree_Wolf::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);
	CM_Monster::Collision_ToPlayer();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Tree_Wolf::Render()
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

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CM_Tree_Wolf::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Tree_Witch"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.5f, 1.7f, 1.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Tree_Wolf::SetUp_ShaderResources()
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

void CM_Tree_Wolf::Monster_Tick(const _double& TimeDelta)
{
	if (0 >= m_tMonsterInfo.iHp)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(5, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(10, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(7, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick();
		m_pModelCom->Set_AnimIndex(2, false);
		break;
	}

	// 1 : �� / 6 �Ͼ�� / 7 : ������ / 8 : ���غ� / 9 : ������ ���� �ڴ�.
}

void CM_Tree_Wolf::Idle_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 5.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;
}

void CM_Tree_Wolf::Find_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Tree_Wolf::Move_Tick(const _double& TimeDelta)
{
	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
	m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta, 1.f);

	if (0.9f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
	}
}

void CM_Tree_Wolf::Attack_Tick(const _double& TimeDelta)
{
	_int iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(0, 1);
	iRandomNum = 1; // Temp
	if (0 == iRandomNum)
		m_pModelCom->Set_AnimIndex(0, false);	// ����
	else
		m_pModelCom->Set_AnimIndex(9, false);	// ������ ���� ����

	if (m_pModelCom->Animation_Check(0))
	{

	}
	if (m_pModelCom->Animation_Check(9))
	{
		if (m_pModelCom->Get_Finished())
		{
			m_pModelCom->Set_AnimIndex(7, false);	// ������
		}
	}
	if (m_pModelCom->Animation_Check(7))
	{
		if (m_pModelCom->Get_Finished())
		{
			m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
		}
	}

}

void CM_Tree_Wolf::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
}

void CM_Tree_Wolf::Die_Tick()
{
	// ���Ͱ� �װ� ���� �� �ൿ

	CGameObject::Set_Dead();
	CObj_Manager::GetInstance()->Set_Player_Exp(m_tMonsterInfo.iExp);	// �÷��̾�� ����ġ ����

	if (!m_OneCoin)	// ���� ����
	{
		// Item
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), 15, 7, 5);

		m_OneCoin = true;
	}
}

CM_Tree_Wolf * CM_Tree_Wolf::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Tree_Wolf*		pInstance = new CM_Tree_Wolf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Tree_Wolf");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Tree_Wolf::Clone(void * pArg)
{
	CM_Tree_Wolf*		pInstance = new CM_Tree_Wolf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Tree_Wolf");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Tree_Wolf::Free()
{
	__super::Free();


}