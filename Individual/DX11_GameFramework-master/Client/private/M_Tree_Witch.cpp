#include "stdafx.h"
#include "..\public\M_Tree_Witch.h"

#include "GameInstance.h"
#include "Obj_Manager.h"		// 플레이어 정보 얻어오려고..
#include "ItemManager.h"		// 죽을 때 동전 터트릴려고..
#include "Utilities_Manager.h"	// 랜덤값 쓰려고..
#include "Effect_Manager.h"
#include "UI_Manager.h"

#include "E_DieCenter.h"
#include "UI_3DTexture.h"		// 느낌표 띄우려고...
#include "B_AnimBullet.h"		// 덩굴

CM_Tree_Witch::CM_Tree_Witch(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Tree_Witch::CM_Tree_Witch(const CM_Tree_Witch & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Tree_Witch::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Tree_Witch::Initialize(void * pArg)
{
	m_wsTag = L"Monster__TreeWitch";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind					= MonsterDesc.eMonsterKind;
	MonsterDesc.TransformDesc.fSpeedPerSec		= 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec	= XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos				= _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_tMonsterInfo.eState	= m_tMonsterInfo.IDLE;
	m_tMonsterInfo.fHP		= 100.0f;
	m_tMonsterInfo.fMaxHP	= 70.0f;
	m_tMonsterInfo.fExp		= 50.0f;
	m_tMonsterInfo.fAttack	= 12.0f;

	return S_OK;
}

void CM_Tree_Witch::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Monster_Tick(TimeDelta);
}

void CM_Tree_Witch::Late_Tick(_double TimeDelta)
{


	__super::Late_Tick(TimeDelta);
}

HRESULT CM_Tree_Witch::Render()
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
			if (m_bShader_Hit)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CM_Tree_Witch::On_Collision(CGameObject * pOther)
{
	CM_Monster::On_Collision(pOther);
}

HRESULT CM_Tree_Witch::SetUp_Components()
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
	ColliderDesc.vSize = _float3(1.2f, 1.7f, 1.2f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Tree_Witch::SetUp_ShaderResources()
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

void CM_Tree_Witch::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		m_pModelCom->Set_AnimIndex(5, false, false);
		Idle_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::MOVE:
		m_pModelCom->Set_AnimIndex(10, false, false);
		Move_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::FIND:
		m_pModelCom->Set_AnimIndex(8, false);
		Find_Tick();
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::HIT:
		m_pModelCom->Set_AnimIndex(4, false);
		Hit_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::DIE:
		m_pModelCom->Set_AnimIndex(2, false);
		Die_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(1);
		break;
	}

	// 1 : 춤 / 6 일어나기 / 7 : 누르기 / 8 : 뛸준비 / 9 : 누르기 위해 뛴다.
}

void CM_Tree_Witch::Idle_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 4.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	if (m_bAttack && m_pModelCom->Get_Finished())
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;
		m_bAttack = false;
	}
}

void CM_Tree_Witch::Find_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_FIND;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 2.f, f4MyPos.z - 0.5f);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Find_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Tree_Witch::Move_Tick(const _double& TimeDelta)
{
	if (4.f < CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	_int iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(0, 25);

	if (0 == iRandomNum)		// 덩굴 생성
	{
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta, 2.f);

		if (3.f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		{
			_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
			XMStoreFloat4(&m_f4PlayerPos, vPlayerPos);

			m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
			m_pModelCom->Set_AnimIndex(0, false);
		}
	}		
	else						// 깔아 뭉개기	
	{
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta, 1.f);

		if (1.f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		{
			m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
			m_pModelCom->Set_AnimIndex(8, false, false);
			m_eSkill_AnimState = JUMP;
		}
	}
}

void CM_Tree_Witch::Attack_Tick(const _double& TimeDelta)
{
	m_bAttack = true;

	// 덩굴
	if (m_pModelCom->Animation_Check(0) && m_pModelCom->Get_Finished())
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;	

		CB_AnimBullet::ANIMBULLETINFO	tBulletInfo;
		tBulletInfo.eBulletType = tBulletInfo.TYPE_ROOTS;
		tBulletInfo.fMonsterAttack = m_tMonsterInfo.fAttack;
		tBulletInfo.f3Start_Pos = _float3(m_f4PlayerPos.x, m_f4PlayerPos.y, m_f4PlayerPos.z);
		
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_B_RandomBullet_Roots_0"), TEXT("Prototype_GameObject_B_AnimBullet"), &tBulletInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	// 깔아 뭉개기
	else
	{
		Attack_Tick2(TimeDelta);
	}
}

void CM_Tree_Witch::Attack_Tick2(const _double & TimeDelta)
{
	switch (m_eSkill_AnimState)
	{
	case Client::CM_Tree_Witch::JUMP:
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::TREEWITCH_0);
		m_pModelCom->Set_AnimIndex(9, false, false);   // 누르기 위해 점프
		break;
	case Client::CM_Tree_Witch::PRESSURE:
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::TREEWITCH_1);
		m_pModelCom->Set_AnimIndex(7, false, false);   // 누르기
		break;
	case Client::CM_Tree_Witch::RISE:
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::TREEWITCH_2);
		m_pModelCom->Set_AnimIndex(6, false, false);   // 일어나기
		break;
	}

	if (9 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_eSkill_AnimState = PRESSURE;
	if (7 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_eSkill_AnimState = RISE;
	if (6 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	{
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::IDLE);
		m_eSkill_AnimState = STATE_END;
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}
}

void CM_Tree_Witch::Hit_Tick(const _double& TimeDelta)
{
	CM_Monster::Effect_Hit(_float3(0.0f, 1.3f, -1.0f));

	m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.2f);
	
	if (m_pModelCom->Get_Finished())
	{
		m_bShader_Hit = false;
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}
}

void CM_Tree_Witch::Die_Tick(const _double& TimeDelta)
{
	//CM_Monster::Die(TimeDelta, 1.2f, 10, 3, 2);

	// 몬스터가 죽고 나면 할 행동

	if (0.0f >= m_fAlpha)
	{
		// 몬스터 죽으면 UI 초기화
		CUI_Manager::GetInstance()->Set_HPGauge_Monster(1.0f);
		//CObj_Manager::GetInstance()->Set_Monster_Crash(false);

		// 알파값이 다 사라지면 죽음
		CGameObject::Set_Dead();
	}

	if (0.0f < m_fAlpha)															// 알파값 줄어 들도록
		m_fAlpha -= _float(TimeDelta);

	if (5 != m_iDieEffect_Count)													// 이펙트 5개
	{
		++m_iDieEffect_Count;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CE_DieCenter::DIECENTERINFO tDieCenterInfo;									
		tDieCenterInfo.eMonsterKind = CE_DieCenter::DIECENTERINFO::BROWN;
		tDieCenterInfo.f3Pos = _float3(vf4MyPos.x, vf4MyPos.y + 1.5f, vf4MyPos.z - 1.f);
		CEffect_Manager::GetInstance()->DieCenter_Create(tDieCenterInfo);
	}

	if (!m_OneCoin)															// 한 번만
	{
		CObj_Manager::GetInstance()->Set_Player_PlusExp(m_tMonsterInfo.fExp);
		CUI_Manager::GetInstance()->Set_LevelGauge_Player(m_tMonsterInfo.fExp);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), 10, 3, 2); 	// 동전 생성

		m_OneCoin = true;
	}
}

CM_Tree_Witch * CM_Tree_Witch::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Tree_Witch*		pInstance = new CM_Tree_Witch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Tree_Witch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Tree_Witch::Clone(void * pArg)
{
	CM_Tree_Witch*		pInstance = new CM_Tree_Witch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Tree_Witch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Tree_Witch::Free()
{
	__super::Free();


}