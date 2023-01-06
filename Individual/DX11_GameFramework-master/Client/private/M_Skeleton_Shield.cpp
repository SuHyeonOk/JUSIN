#include "stdafx.h"
#include "..\public\M_Skeleton_Shield.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"

#include "W_Skeleton_Shield.h"
#include "Bone.h"

CM_Skeleton_Shield::CM_Skeleton_Shield(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Skeleton_Shield::CM_Skeleton_Shield(const CM_Skeleton_Shield & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Skeleton_Shield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Skeleton_Shield::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Skeleton_Shield";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	MonsterDesc.TransformDesc.fSpeedPerSec = 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
	m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
	m_tMonsterInfo.fHP		= 80.0f;
	m_tMonsterInfo.fMaxHP	= 80.0f;
	m_tMonsterInfo.fExp		= 70.0f;
	m_tMonsterInfo.fAttack	= 25.0f;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	return S_OK;
}

void CM_Skeleton_Shield::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);

	if (m_tMonsterInfo.ATTACK == m_tMonsterInfo.eState)
		m_MonsterParts[0]->Tick(TimeDelta);
	m_MonsterParts[1]->Tick(TimeDelta);
}

void CM_Skeleton_Shield::Late_Tick(_double TimeDelta)
{
	if(m_tMonsterInfo.ATTACK == m_tMonsterInfo.eState)
		m_MonsterParts[0]->Late_Tick(TimeDelta);
	m_MonsterParts[1]->Late_Tick(TimeDelta);

	__super::Late_Tick(TimeDelta);
}

HRESULT CM_Skeleton_Shield::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_tMonsterInfo.eState == m_tMonsterInfo.DIE)
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

			if (i == 0)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
		}

		return S_OK;	// 죽었다면, 여기까지 진행하고 return
	}

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 0)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CM_Skeleton_Shield::On_Collision(CGameObject * pOther)
{
  	if (!m_bDefense)
		CM_Monster::On_Collision(pOther);
	else
		m_bDefense = false;
}

HRESULT CM_Skeleton_Shield::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_SHIELD_1)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_2"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_SHIELD_2)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_4"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 1.2f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CM_Skeleton_Shield::SetUp_ShaderResources()
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

	if (m_tMonsterInfo.eState == m_tMonsterInfo.DIE)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CM_Skeleton_Shield::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CW_Skeleton_Shield::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CW_Skeleton_Shield::WEAPONDESC));

	WeaponDesc.eWarriorType = WeaponDesc.SWORD;
	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_sword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_W_Skeleton_Shield"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);

	WeaponDesc.eWarriorType = WeaponDesc.SHIELD;
	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_shield");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_W_Skeleton_Shield"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CM_Skeleton_Shield::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	// 0 : ATTACK
	// 1 : 반대로 공격
	// 2 : HIT
	// 3 : 방패막기
	// 4 : DIE
	// 5 : 죽은채로 가만히
	// 6 : 살아남
	// 7 : 춤
	// 8 : 목 돌아감
	// 9 : HIT
	// 10 : 똑같은 히트
	// 11 : IDLE
	// 12 : MOVE

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(11, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(12, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(8, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(7);
		break;
	}
}

void CM_Skeleton_Shield::Idle_Tick(const _double& TimeDelta)
{
	// IDLE 일 때, MOVE 일 때 똑같이 거리 이내 플레이어가 있는지 확인한다.
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.5f > fDistance)	// ※ 플레이어 탐색 범위		
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	// 없다면 IDLE 과 MOVE 를 번갈아 가며 실행한다.
	if(m_pModelCom->Animation_Check(11) && m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Skeleton_Shield::Move_Tick(const _double& TimeDelta)
{
	// 1 : 플레이어를 찾았을 때의 MOVE
	// 2 : 플레이어를 찾지 못 하고 랜덤으로 이동하는 MOVE

	if (m_bFind)		// 플레이어를 찾았을 때! 플레이어와 거리가 1이 될 때 까지 이동한다.
	{
		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta);

		// 거리가 4 안 이라면 플레이어를 따라가서 거리가 1이 될 때 공격한다.
		if (2.f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
			m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

		// ※ 플레이어 포기 범위
		if (4.f < CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
			m_bFind = false; // 크흠.. 범위 벗어났을 때 막 뱅글뱅글 돌면서 회전하는 문제 있음..

	}
	else				// 플레이어를 찾지 못 했을 때 랜덤으로 이동하고 있는다
	{
		// MOVE 일 때, IDLE 일 때 똑같이 거리 이내 플레이어가 있는지 확인한다.
		_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		if (!m_bAttack && 3.5f > fDistance)	// ※ 플레이어 탐색 범위
			m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

		// 내 원점 거리와 내 위치가 멀다면! 무조건 원점으로 돌아간다.
		_vector	vCenterPos = XMLoadFloat4(&m_f4CenterPos);
		_vector vDistance = vCenterPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float	fDiatance = XMVectorGetX(XMVector3Length(vDistance));

		if (2.1f < fDiatance)
		{
			m_pTransformCom->Chase(vCenterPos, TimeDelta);
			m_pTransformCom->LookAt(vCenterPos);
		}
		else
		{
			if (!CM_Monster::Random_Move(m_pTransformCom, m_f4CenterPos, TimeDelta))
			{
				m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
				m_bAttack = false;
			}
		}
	}
}

void CM_Skeleton_Shield::Find_Tick()
{
	m_bFind = true;	// 플레이어를 찾았다면 플레이어에게 다가가기 위해서 MOVE로 이동한다.

	if(m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Skeleton_Shield::Attack_Tick(const _double& TimeDelta)
{
	m_bFind = false;

	if (m_pModelCom->Get_Finished())
	{
		m_bAttack = true;
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
	}
}

void CM_Skeleton_Shield::Hit_Tick()
{
	if (0 == m_iRandomNum)
		m_iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(1, 2);

	if (1 == m_iRandomNum)
	{
		m_pModelCom->Set_AnimIndex(3, false);	// 방어
		if (m_pModelCom->Get_Finished())
		{
			m_iRandomNum = 0;
			m_bDefense = true;
			m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
		}
	}
	else
	{
		m_pModelCom->Set_AnimIndex(2, false);	// Hit
		if (m_pModelCom->Get_Finished())
		{
			m_iRandomNum = 0;
			m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
		}
	}
}

void CM_Skeleton_Shield::Die_Tick(const _double& TimeDelta)
{
	CM_Monster::Die(TimeDelta, 1.1f, 7, 5, 2);
}

CM_Skeleton_Shield * CM_Skeleton_Shield::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Skeleton_Shield*		pInstance = new CM_Skeleton_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Skeleton_Shield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Skeleton_Shield::Clone(void * pArg)
{
	CM_Skeleton_Shield*		pInstance = new CM_Skeleton_Shield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Skeleton_Shield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Skeleton_Shield::Free()
{
	__super::Free();

	for (auto& pPart : m_MonsterParts)
		Safe_Release(pPart);
	m_MonsterParts.clear();
}