#include "stdafx.h"
#include "..\public\M_Skeleton_Archer.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"

#include "E_DieCenter.h"
#include "B_3DBullet.h"

CM_Skeleton_Archer::CM_Skeleton_Archer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Skeleton_Archer::CM_Skeleton_Archer(const CM_Skeleton_Archer & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Skeleton_Archer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Skeleton_Archer::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Skeleton_Archer";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(CM_Monster::MONSTERDESC));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	MonsterDesc.TransformDesc.fSpeedPerSec = 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = MonsterDesc.f3Pos;
	m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.MOVE;
	m_tMonsterInfo.fHP		= 130.0f;
	m_tMonsterInfo.fMaxHP	= m_tMonsterInfo.fHP;
	m_tMonsterInfo.fExp		= 15.0f;
	m_tMonsterInfo.fAttack	= 50.0f;

	return S_OK;
}

void CM_Skeleton_Archer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	BossCage();
	Monster_Tick(TimeDelta);
}

void CM_Skeleton_Archer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 공격 받고 있을 때, 죽을 때, 케이지 안에 있을 때 Xray 을 호출하지 않는다.
	if (true == m_bShader_Hit || 1 != m_fAlpha || true == CObj_Manager::GetInstance()->Get_BossCage())
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_XRAYBLEND, this);
	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CM_Skeleton_Archer::Render()
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
		{
			if (m_bShader_Hit)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
		}
	}

	return S_OK;
}

HRESULT CM_Skeleton_Archer::Render_XRay()
{
	if (FAILED(__super::Render_XRay()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderXRayResources()))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderXRayCom, 1, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderXRayCom, 1, "g_BoneMatrices");

	return S_OK;
}

void CM_Skeleton_Archer::On_Collision(CGameObject * pOther)
{
	__super::On_Collision(pOther);
}

HRESULT CM_Skeleton_Archer::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_ShaderXRay */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel_XRay"), TEXT("Com_ShaderXRay"),
		(CComponent**)&m_pShaderXRayCom)))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_ARCHER_1)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_3"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_ARCHER_2)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_4"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_ARCHER_3)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_2"), TEXT("Com_Model"),
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

HRESULT CM_Skeleton_Archer::SetUp_ShaderResources()
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

HRESULT CM_Skeleton_Archer::SetUp_ShaderXRayResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderXRayCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float	fObjectID = 3.0f;
	if (FAILED(m_pShaderXRayCom->Set_RawValue("g_ObjectID", &fObjectID, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

void CM_Skeleton_Archer::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	// 0 : 안 에서 던지기
	// 1 : 밖에서 던지기
	// 2 : 죽음
	// 3 : 넘어져 있음
	// 4 : 일어남
	// 5 : 춤ㅈㅁ
	// 6 : 목돌아감
	// 7 : 히트
	// 8 : 이거 히트
	// 9 : 아이들
	// 10 : 걷기

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(9, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(10, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(6, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(8, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(2, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(5);
		break;
	}
}

void CM_Skeleton_Archer::Idle_Tick(const _double& TimeDelta)
{
	m_bIdle_Sound = false;
	
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 5.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	//if (m_bAttack && 50 <= m_pModelCom->Get_Keyframes())
	//	m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Skeleton_Archer::Move_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	
	if (7.0f > fDistance)
	{
		if (false == m_bIdle_Sound)
		{
			m_bIdle_Sound = true;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("Skeleton_Idle.mp3"), 0.7f);
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	
	if (!m_bAttack && 5.f > fDistance)
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

void CM_Skeleton_Archer::Find_Tick()
{
	m_bIdle_Sound = false;

	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Skeleton_Archer::Attack_Tick(const _double& TimeDelta)
{
	if (m_pModelCom->Get_Finished())
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
		m_bAttack = true;

		// 내 좌표
		_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 플레이어 좌표
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4	f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		CB_3DBullet::NONANIMBULLETINFO	tBulletInfo;
		tBulletInfo.fMonsterAttack = m_tMonsterInfo.fAttack;
		tBulletInfo.eBulletType = tBulletInfo.TYPE_SKELETON;
		tBulletInfo.f3Start_Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		tBulletInfo.f3Target_Pos = _float3(f4PlayerPos.x, f4PlayerPos.y + 0.7f, f4PlayerPos.z);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_B_RandomBullet_Skeleton_0"), TEXT("Prototype_GameObject_B_RandomBullet"), &tBulletInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CM_Skeleton_Archer::Hit_Tick(const _double& TimeDelta)
{
	CM_Monster::Effect_Hit();
	m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.2f);

	if (m_pModelCom->Get_Finished())
	{
		m_bShader_Hit = false;
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
	}
}

void CM_Skeleton_Archer::Die_Tick(const _double& TimeDelta)
{
	if(LEVEL_SKELETON_BOSS != CObj_Manager::GetInstance()->Get_Current_Level())
		CM_Monster::Die(TimeDelta, 1.1f, 15, 6, 5);
	else
		CM_Monster::Die(TimeDelta, 1.1f, 0, 0, 0);
}

void CM_Skeleton_Archer::BossCage()
{
	if (false == CObj_Manager::GetInstance()->Get_BossCage())
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4Pos, vPos);

	if (1.0f > f4Pos.x || 9.5f < f4Pos.x || 14.0f > f4Pos.z || 18.0f < f4Pos.z)
	{
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook *= -1.0f;
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
}

CM_Skeleton_Archer * CM_Skeleton_Archer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Skeleton_Archer*		pInstance = new CM_Skeleton_Archer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Skeleton_Archer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Skeleton_Archer::Clone(void * pArg)
{
	CM_Skeleton_Archer*		pInstance = new CM_Skeleton_Archer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Skeleton_Archer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Skeleton_Archer::Free()
{
	__super::Free();

	Safe_Release(m_pShaderXRayCom);
}