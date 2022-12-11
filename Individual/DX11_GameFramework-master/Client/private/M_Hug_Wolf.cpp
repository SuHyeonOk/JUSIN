#include "stdafx.h"
#include "..\public\M_Hug_Wolf.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Skill_Manager.h"

CM_Hug_Wolf::CM_Hug_Wolf(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Hug_Wolf::CM_Hug_Wolf(const CM_Hug_Wolf & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Hug_Wolf::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Hug_Wolf::Initialize(void * pArg)
{
	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	MonsterDesc.TransformDesc.fSpeedPerSec = 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);

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

void CM_Hug_Wolf::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		// TODO : 충돌처리가 가능해 지면 수정 (둘 다 한 번만 호출 되어야 함)
		m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
		m_tMonsterInfo.eState = m_tMonsterInfo.HIT;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CM_Hug_Wolf::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Hug_Wolf::Render()
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

HRESULT CM_Hug_Wolf::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Hug_Wolf"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Hug_Wolf::SetUp_ShaderResources()
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

void CM_Hug_Wolf::Monster_Tick(const _double& TimeDelta)
{
	//_matrix PlayerWorld;
	//PlayerWorld = m_pTransformCom->Get_WorldMatrix();
	//_float4x4 f44PlayerWorld;
	//XMStoreFloat4x4(&f44PlayerWorld, PlayerWorld);
	//cout << "World_Right	: " << f44PlayerWorld._11 << " | " << f44PlayerWorld._12 << " | " << f44PlayerWorld._13 << " | " << f44PlayerWorld._14 << endl;
	//cout << "World_Up		: " << f44PlayerWorld._21 << " | " << f44PlayerWorld._22 << " | " << f44PlayerWorld._23 << " | " << f44PlayerWorld._24 << endl;
	//cout << "World_Look		: " << f44PlayerWorld._31 << " | " << f44PlayerWorld._32 << " | " << f44PlayerWorld._33 << " | " << f44PlayerWorld._34 << endl;
	//cout << "World_Pos		: " << f44PlayerWorld._41 << " | " << f44PlayerWorld._42 << " | " << f44PlayerWorld._43 << " | " << f44PlayerWorld._44 << endl;
	//cout << "----------------------------------------" << endl;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, -1.f, 0.f));

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
		m_pModelCom->Set_AnimIndex(8, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(1, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		m_pModelCom->Set_AnimIndex(6, false);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick();
		m_pModelCom->Set_AnimIndex(4, false);
		break;
	}
}

void CM_Hug_Wolf::Idle_Tick(const _double& TimeDelta)
{
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 5.f > fDistance)
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;
}

void CM_Hug_Wolf::Find_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	//m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Hug_Wolf::Move_Tick(const _double& TimeDelta)
{
	//m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
	m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta, 1.f);

	if (0.9f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
	}
}

void CM_Hug_Wolf::Attack_Tick(const _double& TimeDelta)
{
	if (m_pModelCom->Get_Finished())
	{
		m_pModelCom->Set_AnimIndex(1, false);
		if (m_pModelCom->Get_Finished())
		{
			m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
		}
	}



}

void CM_Hug_Wolf::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
}

void CM_Hug_Wolf::Die_Tick()
{
	// 몬스터가 죽고 나면 할 행동

	CObj_Manager::GetInstance()->Set_Player_Exp(m_tMonsterInfo.iExp);	// 플레이어에게 경험치 증가

	if (!m_OneCoin)	// 동전 생성
	{
		// Item
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vf4MyPos;
		XMStoreFloat4(&vf4MyPos, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x + 2.f, vf4MyPos.y, vf4MyPos.z), 15, 7, 5);

		m_OneCoin = true;
	}
}

CM_Hug_Wolf * CM_Hug_Wolf::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Hug_Wolf*		pInstance = new CM_Hug_Wolf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Hug_Wolf");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Hug_Wolf::Clone(void * pArg)
{
	CM_Hug_Wolf*		pInstance = new CM_Hug_Wolf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Hug_Wolf");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Hug_Wolf::Free()
{
	__super::Free();


}