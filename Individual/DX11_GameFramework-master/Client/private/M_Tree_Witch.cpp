#include "stdafx.h"
#include "..\public\M_Tree_Witch.h"

#include "GameInstance.h"
#include "Obj_Manager.h"		// 플레이어 정보 얻어오려고..
#include "ItemManager.h"		// 죽을 때 동전 터트릴려고..
#include "Utilities_Manager.h"	// 랜덤값 쓰려고..

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

void CM_Tree_Witch::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Monster_Tick(TimeDelta);
}

void CM_Tree_Witch::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Tree_Witch::Render()
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
	ColliderDesc.vSize = _float3(1.5f, 1.7f, 1.5f);
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

	return S_OK;
}

void CM_Tree_Witch::Monster_Tick(const _double& TimeDelta)
{
	if (0 >= m_tMonsterInfo.iHp)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		m_pModelCom->Set_AnimIndex(5, false);
		Idle_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::MOVE:
		m_pModelCom->Set_AnimIndex(10, false);
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
		Hit_Tick();
		break;

	case MONSTERINFO::STATE::DIE:
		m_pModelCom->Set_AnimIndex(2, false);
		Die_Tick();
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

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_FIND;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 2.f, f4MyPos.z - 0.5f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Find_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Tree_Witch::Move_Tick(const _double& TimeDelta)
{
	if (4.f < CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	_int iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(0, 25);
	iRandomNum = 0;
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
			m_pModelCom->Set_AnimIndex(8, false);
			m_eSkill_AnimState = JUMP;

			// 플레이어의 애니메이션 상태 변경하기
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::TREEWITCH);
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
		
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CB_AnimBullet::ANIMBULLETINFO	tBulletInfo;
		tBulletInfo.eBulletType = tBulletInfo.TYPE_ROOTS;
		tBulletInfo.iMonsterAttack = m_tMonsterInfo.iAttack;
		tBulletInfo.f3Start_Pos = _float3(m_f4PlayerPos.x, m_f4PlayerPos.y, m_f4PlayerPos.z);
		
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_B_RandomBullet_Roots_0"), TEXT("Prototype_GameObject_B_AnimBullet"), &tBulletInfo)))
			return;
		
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
		m_pModelCom->Set_AnimIndex(9, false, false);   // 누르기 위해 점프
		break;
	case Client::CM_Tree_Witch::PRESSURE:
		m_pModelCom->Set_AnimIndex(7, false, false);   // 누르기
		break;
	case Client::CM_Tree_Witch::RISE:
		m_pModelCom->Set_AnimIndex(6, false, false);   // 일어나기
		break;
	}

	if (9 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_eSkill_AnimState = PRESSURE;
	if (7 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_eSkill_AnimState = RISE;
	if (6 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	{
		m_eSkill_AnimState = STATE_END;
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}

	//switch (m_eState)
	//{
	//case CSkill_Manager::MONSTERSKILL::TREEWITCH::JUMP:
	//	m_pModelCom->Set_AnimIndex(9, false, false);   // 누르기 위해 점프
	//	break;

	//case CSkill_Manager::MONSTERSKILL::TREEWITCH::PRESSURE:
	//	m_pModelCom->Set_AnimIndex(7, false);   // 누르기
	//	break;

	//case CSkill_Manager::MONSTERSKILL::TREEWITCH::RISE:
	//	m_pModelCom->Set_AnimIndex(6, false);   // 일어나기
	//	break;
	//}

	//CSkill_Manager::GetInstance()->Set_TreeWitch_Skill(m_eState);



	////if (9 == m_pModelCom->Get_AnimIndex())
	////	cout << m_pModelCom->Get_Keyframes() << endl;
	//if (9 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	//	m_eState = CSkill_Manager::MONSTERSKILL::TREEWITCH::PRESSURE;
	//if (7 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	//	m_eState = CSkill_Manager::MONSTERSKILL::TREEWITCH::RISE;
	//if (6 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	//{
	//	CSkill_Manager::GetInstance()->Set_TreeWitch_Skill(CSkill_Manager::MONSTERSKILL::TREEWITCH::TREEWITCH_END);
	//	m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	//}
}

void CM_Tree_Witch::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
}

void CM_Tree_Witch::Die_Tick()
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

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), 10, 3, 2);

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