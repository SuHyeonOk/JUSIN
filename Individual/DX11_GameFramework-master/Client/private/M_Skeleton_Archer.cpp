#include "stdafx.h"
#include "..\public\M_Skeleton_Archer.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"

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
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
	m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.MOVE;
	m_tMonsterInfo.iHp		= 70;
	m_tMonsterInfo.iExp		= 70;
	m_tMonsterInfo.iAttack	= 20;

	return S_OK;
}

void CM_Skeleton_Archer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	cout << m_i << endl;

	if (pGameInstance->Key_Down(DIK_P))
	{
		m_i++;
	}
	if (pGameInstance->Key_Down(DIK_O))
	{
		m_i--;
	}
	m_pModelCom->Set_AnimIndex(m_i, false);

	// 0 : 발견
	// 1 : 조이기
	// 2 : 놓치기
	// 3 : 발 춤추기
	// 4 : 뒤로 넘어갈랑
	// 5 : 오 Hit
	// 6 : 왼 Hit
	// 7 : IDLE
	// 8 : MOVE
	// 9 : 조이기2
	// 10 : 앞으로 한 발 뛰고 공중
	// 11 : 뒤로 한 발 뛰고 공중
	// 12 : 황소 뒤로 발차듯이
	// 13 : 이상한 껑충
	// 14 : 막기
	// 15 : 막다가 Hit

	RELEASE_INSTANCE(CGameInstance);
//	Monster_Tick(TimeDelta);
}

void CM_Skeleton_Archer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Skeleton_Archer::Render()
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

	return S_OK;
}

void CM_Skeleton_Archer::Monster_Tick(const _double& TimeDelta)
{
	if (0 >= m_tMonsterInfo.iHp)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(7, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(9, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		m_pModelCom->Set_AnimIndex(5, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick();
		m_pModelCom->Set_AnimIndex(3, false);
		break;
	}
}

void CM_Skeleton_Archer::Idle_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	if (m_bAttack && 50 <= m_pModelCom->Get_Keyframes())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Skeleton_Archer::Move_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	if (!CM_Monster::Random_Move(m_pTransformCom, m_f4CenterPos, TimeDelta, 2.5))
	{
		m_bAttack = false;
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}
}

void CM_Skeleton_Archer::Find_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Skeleton_Archer::Attack_Tick(const _double& TimeDelta)
{
	_int	iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(0, 1);
	if (0 != iRandomNum && m_pModelCom->Get_Finished())	// 랜덤으로 0이 들어오면 바로 MOVE로 가고, 1일 때는 ATTACK 이다.
		return;

	m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	m_bAttack = true;

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

void CM_Skeleton_Archer::Hit_Tick()
{
 	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Skeleton_Archer::Die_Tick()
{
	// 몬스터가 죽고 나면 할 행동

	CGameObject::Set_Dead();
	CObj_Manager::GetInstance()->Set_Player_Exp(m_tMonsterInfo.iExp);	// 플레이어에게 경험치 증가

	if (!m_OneCoin)	// 동전 생성
	{
		// Item
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), 6, 4, 2);

		m_OneCoin = true;
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


}