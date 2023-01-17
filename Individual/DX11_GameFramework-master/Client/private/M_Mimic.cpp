#include "stdafx.h"
#include "..\public\M_Mimic.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"

#include "E_DieCenter.h"
#include "B_2DBullet.h"
#include "UI_3DTexture.h"

CM_Mimic::CM_Mimic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Mimic::CM_Mimic(const CM_Mimic & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Mimic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Mimic::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Mimic";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;
	MonsterDesc.TransformDesc.fSpeedPerSec = 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	m_tMonsterInfo.fHP = 50.0f;
	m_tMonsterInfo.fMaxHP = 50.0f;
	m_tMonsterInfo.fExp = 50.0f;
	m_tMonsterInfo.fAttack = 15.0f;

	return S_OK;
}

void CM_Mimic::Tick(_double TimeDelta)
{
	//__super::Tick(TimeDelta);

	Hit_Process(TimeDelta);
	Monster_Tick(TimeDelta);
}

void CM_Mimic::Late_Tick(_double TimeDelta)
{


	__super::Late_Tick(TimeDelta);
}

HRESULT CM_Mimic::Render()
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
			if (1 == i)
				continue;

			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
		}

		return S_OK;
	}

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (1 == i)
			continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CM_Mimic::On_Collision(CGameObject * pOther)
{
	CM_Monster::On_Collision(pOther);

	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		if (m_tMonsterInfo.ATTACK == m_tMonsterInfo.eState)
		{
			CObj_Manager::GetInstance()->Set_Player_MinusHP(m_tMonsterInfo.fAttack);
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::HIT);
		}
	}
}

HRESULT CM_Mimic::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Mimic"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 0.5f, 1.0f);
	ColliderDesc.vCenter = _float3(-3.6f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Mimic::SetUp_ShaderResources()
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

void CM_Mimic::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(8, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(9, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(1, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(3, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(2);
		break;
	}
}

void CM_Mimic::Idle_Tick(const _double& TimeDelta)
{
	if (m_bPlayer_Attack)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;
}

void CM_Mimic::Find_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_FIND;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x - 3.6f, f4MyPos.y + 1.3f, f4MyPos.z - 0.5f);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Find_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Mimic::Attack_Tick(const _double& TimeDelta)
{
	CObj_Manager::GetInstance()->Set_Interaction(true);

	if (!m_bPlayer_Attack)
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
}

void CM_Mimic::Hit_Tick(const _double& TimeDelta)
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
}

void CM_Mimic::Die_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_fAlpha)
	{
		CUI_Manager::GetInstance()->Set_HPGauge_Monster(1.0f);
		CUI_Manager::GetInstance()->Set_Ui_Monster(false);

		CGameObject::Set_Dead();
	}

	if (0.0f < m_fAlpha)															
		m_fAlpha -= _float(TimeDelta);

	if (5 != m_iDieEffect_Count)													
	{
		++m_iDieEffect_Count;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CE_DieCenter::DIECENTERINFO tDieCenterInfo;
		tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::RED;
		tDieCenterInfo.f3Pos = _float3(vf4MyPos.x - 3.6f, vf4MyPos.y + 0.7f, vf4MyPos.z - 0.5f);
		CEffect_Manager::GetInstance()->DieCenter_Create(tDieCenterInfo);
	}

	if (!m_OneCoin)														
	{
		CObj_Manager::GetInstance()->Set_Player_PlusExp(m_tMonsterInfo.fExp);
		CUI_Manager::GetInstance()->Set_LevelGauge_Player(m_tMonsterInfo.fExp);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x - 3.6f, vf4MyPos.y, vf4MyPos.z), 10, 3, 2); 	// ???? ????
		CItemManager::GetInstance()->RandomPage_Clone(_float3(vf4MyPos.x - 3.6f, vf4MyPos.y, vf4MyPos.z));

		m_OneCoin = true;
	}
}

void CM_Mimic::Hit_Process(const _double & TimeDelta)
{
	// 미믹 몬스터의 경우 Hit 넉백, 이동이 되면 안 되기에 CM_Mosnter::Tick() 을 돌지 않는다.

	if (!m_bPlayer_Attack)
		return;

	// 몬스터 공격 받는 중...

	// 몬스터 Hit 셰이더 흰색 깜박!
	m_bShader_Hit = true;

	if (0.1 < m_dPlayer_Attack_TimeAcc)
		m_bShader_Hit = false;

	// 몬스터 상태 변경
	m_tMonsterInfo.eState = m_tMonsterInfo.HIT;

	// 맨 처음 한 번 체력을 깍는다.
	if (0 == m_dPlayer_Attack_TimeAcc)
	{
		m_tMonsterInfo.fHP -= CObj_Manager::GetInstance()->Get_Player_Attack();							// 플레이어의 공격력 으로 몬스터 체력 깍기
		CUI_Manager::GetInstance()->Set_HPGauge_Monster(m_tMonsterInfo.fHP / m_tMonsterInfo.fMaxHP);	// UI 에 내 체력 넘겨주기
	}
	// 몬스터 무적 상태 (안 하면 계속 공격 받음)
	m_dPlayer_Attack_TimeAcc += TimeDelta;
	if (0.5 < m_dPlayer_Attack_TimeAcc)
	{
		m_bPlayer_Attack = false;
		m_dPlayer_Attack_TimeAcc = 0;
		return;
	}
}

CM_Mimic * CM_Mimic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Mimic*		pInstance = new CM_Mimic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Mimic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Mimic::Clone(void * pArg)
{
	CM_Mimic*		pInstance = new CM_Mimic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Mimic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Mimic::Free()
{
	__super::Free();


}