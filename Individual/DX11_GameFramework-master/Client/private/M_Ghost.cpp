#include "stdafx.h"
#include "..\public\M_Ghost.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"

#include "Bone.h"
#include "W_GhostFist.h"

CM_Ghost::CM_Ghost(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Ghost::CM_Ghost(const CM_Ghost & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Ghost::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Ghost::Initialize(void * pArg)
{
	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	MonsterDesc.TransformDesc.fSpeedPerSec = 2.5f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
	m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.GHOST_1)
		m_wsTag = L"Monster__Ghost_0";
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.GHOST_2)
		m_wsTag = L"Monster__Ghost_1";
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.GHOST_3)
		m_wsTag = L"Monster__Ghost_2";

	m_tMonsterInfo.eState	= m_tMonsterInfo.IDLE;
	m_tMonsterInfo.fHP		= 30.0f;
	m_tMonsterInfo.fMaxHP	= 60.0f;
	m_tMonsterInfo.fExp		= 30.0f;
	m_tMonsterInfo.fAttack	= 10.0f;

	m_fAlpha = 0.7f;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	return S_OK;
}

void CM_Ghost::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 주먹 무기
	if (0 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_MonsterParts[0]->Tick(TimeDelta);

	// 전체적인 몬스터 상태
	Monster_Tick(TimeDelta);
}

void CM_Ghost::Late_Tick(_double TimeDelta)
{
	if (0 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_MonsterParts[0]->Late_Tick(TimeDelta);

	// Ghost 의 경우 Renderer 그룹이 다르기 때문에 부모의 Lata_Tick 을 호출하지 않는다.
	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
	m_pColliderCom[COLLTYPE_AABB]->Update(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CM_Ghost::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		if (m_bShader_Hit)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
	}

	return S_OK;
}

void CM_Ghost::On_Collision(CGameObject * pOther)
{
	CM_Monster::On_Collision(pOther);
}

HRESULT CM_Ghost::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.GHOST_1)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Ghost_1"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.GHOST_2)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Ghost_2"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.GHOST_3)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Ghost_3"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.7f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Ghost::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Ghost::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CW_GhostFist::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CW_GhostFist::WEAPONDESC));

	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("R_Arm_03");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_W_GhostFist"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	
	m_MonsterParts.push_back(pPartObject);
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CM_Ghost::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	// 0 : 왼쪽공격
	// 1 : 오른쪽 공격
	// 2 : 춤추기
	// 3 : 발로 차고 뒤로 도망
	// 4 : 왼 HIt
	// 5 : 오 Hit
	// 6 : 도리도리ㅔ
	// 7 : IDLE
	// 8 : 왼쪽으로 얼굴 돌리기메
	// 9 : 크크킄ㅋㅋㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴㄴ
	// 10 : 바라보기ㅔ

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(7, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(10, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(9, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(8, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(2);
		break;
	}
}

void CM_Ghost::Idle_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 4.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	if (7 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Ghost::Move_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 4.f > fDistance)
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
			//m_fAlpha = 0.0f;
			m_bAttack = false;
		}
	}
}

void CM_Ghost::Find_Tick(const _double& TimeDelta)
{
	if (0.0f < m_fAlpha)															// 알파값 줄어 들도록
		m_fAlpha -= _float(TimeDelta);
	else
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Ghost::Attack_Tick(const _double& TimeDelta)
{
	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	if (!m_bAttack)
	{
		m_bAttack = true;

		// 플레이어와의 거리가 1 만큼의 랜덤한 위치로 가서 플레이어를 공격한다.
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4PlaterPos;
		XMStoreFloat4(&f4PlaterPos, vPlayerPos);

		_float fRandomNum = CUtilities_Manager::GetInstance()->Get_Random(-0.1f, 0.1f);
		f4PlaterPos.x += fRandomNum;
		fRandomNum = CUtilities_Manager::GetInstance()->Get_Random(-0.1f, 0.1f);
		f4PlaterPos.z += fRandomNum;

		vPlayerPos = XMLoadFloat4(&f4PlaterPos);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPlayerPos);
	}
	else
	{
		if (0.7f > m_fAlpha)															// 알파값 늘어 나도록
			m_fAlpha += _float(TimeDelta);
		else
		{
			if (0 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
				m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
		}
	}
}

void CM_Ghost::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
	{
		m_bShader_Hit = false;
		m_dShader_Hit_TimeAcc = 0;
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
	}
}

void CM_Ghost::Die_Tick(const _double& TimeDelta)
{
	CM_Monster::Die(TimeDelta, 1.2f, 6, 3, 2);
}

CM_Ghost * CM_Ghost::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Ghost*		pInstance = new CM_Ghost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Ghost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Ghost::Clone(void * pArg)
{
	CM_Ghost*		pInstance = new CM_Ghost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Ghost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Ghost::Free()
{
	__super::Free();

	for (auto& pPart : m_MonsterParts)
		Safe_Release(pPart);
	m_MonsterParts.clear();
}