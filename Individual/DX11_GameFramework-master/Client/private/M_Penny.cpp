#include "stdafx.h"
#include "..\public\M_Penny.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"

#include "E_DieCenter.h"
#include "Page.h"

CM_Penny::CM_Penny(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Penny::CM_Penny(const CM_Penny & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Penny::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Penny::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Penny";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(CM_Monster::MONSTERDESC));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	MonsterDesc.TransformDesc.fSpeedPerSec = 3.5f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = MonsterDesc.f3Pos;
	m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.FIND;
	m_tMonsterInfo.fHP		= 70.0f; // 3번 이상으로 때려야 한다.
	m_tMonsterInfo.fMaxHP	= 70.0f;
	m_tMonsterInfo.fExp		= 70.0f;
	m_tMonsterInfo.fAttack	= 20.0f;

	return S_OK;
}

void CM_Penny::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	Monster_Tick(TimeDelta);
}

void CM_Penny::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CM_Penny::Render()
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

void CM_Penny::On_Collision(CGameObject * pOther)
{
	if (MONSTERINFO::STATE::ATTACK == m_tMonsterInfo.eState)
		return;

	CM_Monster::On_Collision(pOther);
}

HRESULT CM_Penny::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Penny"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.7f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Penny::SetUp_ShaderResources()
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

void CM_Penny::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	// 0 : 발견
	// 1 : 바등바등 물건 뺏기?
	// 2 : 쉬기 
	// 3 : 뒤로넘어짐 
	// 4 : 꽁꽁숨어랒 
	// 5 : 동동동 뛰김
	// 6 : 달리기


	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		break;

	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(2, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(5, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(1, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(3, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(3, false);
		break;
	}
}

void CM_Penny::Find_Tick()
{
	// 플레이어와 거리 비교하여 일정 범위 안 으로 들어오면 애니메이션 한	번 변경했다가 돌아다닌다.
	// 한 번 들어온 후 다시 사용되지 않는다.

	_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (3.0f > fDistance)
	{
		m_pModelCom->Set_AnimIndex(0, false);	// 발견
		PennyCopy_Create();

		if (m_pModelCom->Animation_Check(0) && m_pModelCom->Get_Finished())
			m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::MOVE;
	}
	else
		m_pModelCom->Set_AnimIndex(4);			// 뒤 돌아 있기
}

void CM_Penny::Idle_Tick(const _double& TimeDelta)
{
	if (m_pModelCom->Animation_Check(2) && m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::MOVE;
}

void CM_Penny::Move_Tick(const _double& TimeDelta)
{
	// 내 원점 거리와 내 위치가 멀다면! 무조건 원점으로 돌아간다.
	_vector	vCenterPos = XMLoadFloat4(&m_f4CenterPos);
	_vector vDistance = vCenterPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float	fDiatance = XMVectorGetX(XMVector3Length(vDistance));

	if (3.6f < fDiatance)
	{
		m_pTransformCom->Chase(vCenterPos, TimeDelta);
		m_pTransformCom->LookAt(vCenterPos);
	}
	else
	{
		if (!CM_Monster::Random_Move(m_pTransformCom, m_f4CenterPos, TimeDelta, 3.5f))
			m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::IDLE;
	}
}

void CM_Penny::Attack_Tick(const _double& TimeDelta)
{
	// 플레이어의 왼쪽 하단 스킬창의 첫번 째 아이템 하나를 뺏어온다.
	// 그리고 죽을 때 그 아이템을 돌려준다.

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
	m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta);

	if (1.0f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
	{
		CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEM_ONE, false);
		m_ePlayerSkill = CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE);

		if (m_pModelCom->Animation_Check(1) && m_pModelCom->Get_Finished())
		{
			CSkill_Manager::GetInstance()->Set_Page_Penny(m_ePlayerSkill);
			m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::MOVE;
		}
	}
}

void CM_Penny::Hit_Tick(const _double& TimeDelta)
{
	if (MONSTERINFO::STATE::ATTACK == m_tMonsterInfo.eState)
		return;

	// 애니메이션 따로 없이, 체력만 깍이면 된다.

	CM_Monster::Effect_Hit({ 0.0f, 0.5f, -0.7f });
	m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.2f);

	static _int iHitCount;

	if (m_pModelCom->Get_Finished())
	{
		m_bShader_Hit = false;

		if (2 == iHitCount)
		{
			iHitCount = 3;
			m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::ATTACK;
		}
		else
		{
			++iHitCount;
			m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::MOVE;
		}
	}
}

void CM_Penny::Die_Tick(const _double& TimeDelta)
{
	CM_Monster::Die(TimeDelta, 0.7f, 0, 0, 0);

	static _bool bCreate;

	if (false == bCreate)
	{
		bCreate = true;

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CPage::PAGEINFO tPageInfo;
		tPageInfo.fPos = _float3(f4MyPos.x, f4MyPos.y, f4MyPos.z);
		tPageInfo.bJemp = true;

		tPageInfo.ePlayerSkill = m_ePlayerSkill;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_Page_Penny"), TEXT("Prototype_GameObject_Page"), &tPageInfo)))
		{
			MSG_BOX("Failed to Created : PennyPage");
			return;
		}

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_FamilySword"), TEXT("Prototype_GameObject_FamilySword"), &_float3(f4MyPos.x, f4MyPos.y + 0.25f, f4MyPos.z))))
		{
			MSG_BOX("Failed to Created : FamilySword");
			return;
		}
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CM_Penny::PennyCopy_Create()
{
	static _bool bCreate;

	if (false == bCreate)
	{
		bCreate = true;
	
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CEffect_Manager::GetInstance()->Effect_Smoke_Count(_float3(f4MyPos.x, f4MyPos.y + 0.6f, f4MyPos.z - 0.5f), _float3(0.5f, 0.5f, 0.5f), 30, { 0.3f, 2.0f });

		for (_int i = 0; i < 5; ++i)
		{
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_PennyCopy"), TEXT("Prototype_GameObject_PennyCopy"), &_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z))))
			{
				MSG_BOX("Failed to Created : PennyCopy");
				return;
			}
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

CM_Penny * CM_Penny::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Penny*		pInstance = new CM_Penny(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Penny");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Penny::Clone(void * pArg)
{
	CM_Penny*		pInstance = new CM_Penny(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Penny");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Penny::Free()
{
	__super::Free();


}