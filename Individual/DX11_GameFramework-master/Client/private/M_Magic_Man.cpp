#include "stdafx.h"
#include "..\public\M_Magic_Man.h"

#include "GameInstance.h"
#include "Obj_Manager.h"		
#include "ItemManager.h"		
#include "Skill_Manager.h"		
	
#include "B_3DBullet.h"

CM_Magic_Man::CM_Magic_Man(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Magic_Man::CM_Magic_Man(const CM_Magic_Man & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Magic_Man::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Magic_Man::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Magic_Man";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind					= MonsterDesc.eMonsterKind;
	MonsterDesc.TransformDesc.fSpeedPerSec		= 3.f;
	MonsterDesc.TransformDesc.fRotationPerSec	= XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos				= _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
	m_f4CenterPos								= _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.FIND;
	m_tMonsterInfo.iHp		= 70;
	m_tMonsterInfo.iExp		= 70;
	m_tMonsterInfo.iAttack	= 20;

	//m_pTransformCom->Set_Pos(_float3(MonsterDesc.f3Pos.x, 100.f, MonsterDesc.f3Pos.z));	// 처음에는 높이 있어서 보이지 않는다.

	return S_OK;
}

void CM_Magic_Man::Tick(_double TimeDelta)
{
	//__super::Tick(TimeDelta);

	if (m_bPlayer_Attack)
	{
		m_dPlayer_Attack_TimeAcc += TimeDelta;
		if (1.5 < m_dPlayer_Attack_TimeAcc)
		{
			m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
			m_tMonsterInfo.eState = m_tMonsterInfo.HIT;

			m_bPlayer_Attack = false;
			m_dPlayer_Attack_TimeAcc = 0;
		}
	}

	Monster_Tick(TimeDelta);

}

void CM_Magic_Man::Late_Tick(_double TimeDelta)
{


	__super::Late_Tick(TimeDelta);
}

HRESULT CM_Magic_Man::Render()
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

void CM_Magic_Man::On_Collision(CGameObject * pOther)
{
	// 일반 무기 공격은 피해를 입지 않고, 변신된 후의 주먹 공격만 충돌한다.
	if (CSkill_Manager::MAGICSKILL::ATTACK == CSkill_Manager::GetInstance()->Get_Magic_Skill().eSkill)
		if (L"Finn_Magic" == pOther->Get_Tag() || L"Jake_Magic" == pOther->Get_Tag())
			m_bPlayer_Attack = true;

	if (CObj_Manager::PLAYERINFO::STATE::ATTACK == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		if (L"Finn_Weapon" == pOther->Get_Tag() || L"Jake_Weapon" == pOther->Get_Tag())
 			m_tMonsterInfo.eState = m_tMonsterInfo.ADD_1;

	if(L"Skill_PaintWork" == pOther->Get_Tag())
		m_tMonsterInfo.eState = m_tMonsterInfo.ADD_1;

	if (L"Skill_Marceline" == pOther->Get_Tag())
		m_tMonsterInfo.eState = m_tMonsterInfo.DANCE;

	//CM_Monster::On_Collision(pOther);
}

HRESULT CM_Magic_Man::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Magic_Man"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.7f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Magic_Man::SetUp_ShaderResources()
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

void CM_Magic_Man::Monster_Tick(const _double& TimeDelta)
{
	if (0 >= m_tMonsterInfo.iHp)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		m_pModelCom->Set_AnimIndex(6, false);
		Idle_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::MOVE:
		m_pModelCom->Set_AnimIndex(8, false);
		Move_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::FIND:
		m_pModelCom->Set_AnimIndex(7, false);
		Find_Tick();
		break;

	case MONSTERINFO::STATE::ATTACK:
		m_pModelCom->Set_AnimIndex(0, false);
		Attack_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::HIT:
		m_pModelCom->Set_AnimIndex(4, false);
		Hit_Tick();
		break;

	case MONSTERINFO::STATE::ADD_1:
		m_pModelCom->Set_AnimIndex(3, false);
		NoHit_Tick();
		break;

	case MONSTERINFO::STATE::DIE:
		m_pModelCom->Set_AnimIndex(7, false);	// 무언갈 뿌리면서 사라진다.
		Die_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(2);
		break;
	}
}

void CM_Magic_Man::Find_Tick()
{
	// 가장 먼저 플레이어가 주변에 있는지 계속 탐색한다.
	_vector vCenterPos = XMLoadFloat4(&m_f4CenterPos);
	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
	_vector vDistance = vPlayerPos - vCenterPos;
	_float	fDistance = XMVectorGetX(XMVector3Length(vDistance));

	// TODO : 나중에 정확한 생성 위치를 정하면 그 자리에서 cout 으로 확인해 보면서 일정한 거리에서 나타나 도록 수정하기 (높이 때문에 거리가 내가 생각하는 거리가 아님)
	if (5.f > fDistance)
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCenterPos);
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}
}

void CM_Magic_Man::Idle_Tick(const _double& TimeDelta)
{
	// 플레이어가 범위 안 으로 들어오면 FIND 가 아닌 ATTACK 이다. 왜? 이미 위에서 플레이어가 범위 안 으로 들어왔을 때! 깜짝 등장하기 때문에
	if (3.f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
	
	// 플레이어가 범위 안에 없을 때 에는 IDLE 애니메이션이 끝나면 MOVE 로 이동한다.
	if (m_pModelCom->Animation_Check(6) && m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Magic_Man::Move_Tick(const _double& TimeDelta)
{
	// MOVE 상태로 있는데 범위 안에 플레이어가 들어오면 공격한다.!
	if (!m_bAttack && 3.f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

	// 플레이어가 범위 안 으로 들어오지 않는 경우, IDLE 상태로 돌아간다.
	if (!CM_Monster::Random_Move(m_pTransformCom, m_f4CenterPos, TimeDelta, 3, 5))	// 3초 동안 이동하고, 거리 5 범위 이내에서 움직인다.
	{
		m_bAttack = false;
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}
}

void CM_Magic_Man::Attack_Tick(const _double& TimeDelta)
{
	// 플레이어가 범위 안 으로 들어왔다 공격한다.
	// 플레이어 에게 총알을 날린다. 그 총알을 플레이어가 맞으면 플레어는 상태 제어를 받는다.

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	// 애니메이션이 종료되면 플레이어 에게 총알을 날린다.
	if (m_pModelCom->Animation_Check(0) && m_pModelCom->Get_Finished())
	{
		m_bAttack = true;
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

		// 총알 날리는 코드
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		// 내 좌표
		_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 플레이어 좌표
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4	f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		CB_3DBullet::NONANIMBULLETINFO	tBulletInfo;
		tBulletInfo.iMonsterAttack = m_tMonsterInfo.iAttack;
		tBulletInfo.eBulletType = tBulletInfo.TYPE_MAGIC;
		tBulletInfo.f3Start_Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		tBulletInfo.f3Target_Pos = _float3(f4PlayerPos.x, f4PlayerPos.y + 0.7f, f4PlayerPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_B_RandomBullet_Magic_0"), TEXT("Prototype_GameObject_B_RandomBullet"), &tBulletInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CM_Magic_Man::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
}

void CM_Magic_Man::NoHit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
}

void CM_Magic_Man::Die_Tick(const _double& TimeDelta)
{
	CM_Monster::Die(TimeDelta, 1.2f);

	if (!m_OneCoin)	// 종이 생성
	{
		// Item
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomPage_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z));

		m_OneCoin = true;
	}
}

CM_Magic_Man * CM_Magic_Man::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Magic_Man*		pInstance = new CM_Magic_Man(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Magic_Man");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Magic_Man::Clone(void * pArg)
{
	CM_Magic_Man*		pInstance = new CM_Magic_Man(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Magic_Man");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Magic_Man::Free()
{
	__super::Free();

}