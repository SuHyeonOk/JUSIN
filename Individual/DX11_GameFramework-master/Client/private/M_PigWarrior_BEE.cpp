#include "stdafx.h"
#include "..\public\M_PigWarrior_BEE.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"

CM_PigWarrior_BEE::CM_PigWarrior_BEE(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_PigWarrior_BEE::CM_PigWarrior_BEE(const CM_PigWarrior_BEE & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_PigWarrior_BEE::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior_BEE::Initialize(void * pArg)
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

	//애니메이션 m_pModelCom->Set_AnimIndex(); 로 변경 하잖아용... 그런데 Initialize() 에서 Set 한 애니메이션은 잘 동작하는데
	//	Tick 이나 Late_Tick 에서 호출한 Set 으로는 실행이 안 돼요ㅠㅠ 혹시 이런 적 없으시죠..?
	// 애니메이션 잘 돌아가고 있난 Play_Animation 에서 확인해 보는데 제가 Set 한 Index 도 잘 들어오거등요..
	
	//m_pModelCom->Set_AnimIndex(7);

	m_tMonsterInfo.eState	= m_tMonsterInfo.IDLE;
	m_tMonsterInfo.iHp		= 50;
	m_tMonsterInfo.iExp		= 25;
	m_tMonsterInfo.iAttack	= 5;
	
	return S_OK;
}

void CM_PigWarrior_BEE::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);

	Monster_Die();
	ToThe_Player(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		// TODO : 충돌처리가 가능해 지면 수정
		m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CM_PigWarrior_BEE::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_PigWarrior_BEE::Render()
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

HRESULT CM_PigWarrior_BEE::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_BEE"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior_BEE::SetUp_ShaderResources()
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

void CM_PigWarrior_BEE::Monster_Tick(_double TimeDelta)
{


	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::DIE:
		Idle_Tick();
		break;
	}
}

void CM_PigWarrior_BEE::Idle_Tick()
{
	m_pModelCom->Set_AnimIndex(7);
}

void CM_PigWarrior_BEE::Monster_Die()
{
	// 몬스터가 죽고 나면 할 행동

	if (0 >= m_tMonsterInfo.iHp)
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

		CObj_Manager::GetInstance()->Set_Player_Exp(m_tMonsterInfo.iExp);	// 플레이어에게 경험치

		if (!m_OneCoin)	// 동전 생성
		{
			// Item
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 vf4MyPos;
			XMStoreFloat4(&vf4MyPos, vMyPos);

			CItemManager::GetInstance()->RandomCoin_Clone(_float3(vf4MyPos.x, vf4MyPos.y, vf4MyPos.z), 10, 5, 2);

			m_OneCoin = true;
		}
	}

	return;
}

void CM_PigWarrior_BEE::ToThe_Player(const _double & TimeDelta)
{
	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);			// 내 좌표
	_vector		vDir = CObj_Manager::GetInstance()->Get_Player_Transform() - vMyPos;		// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));							// X 값을 뽑아와 거리 확인

	if (fDistanceX < 5.f)
	{
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_pTransformCom->Chase(XMVectorSet(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z, 1.f), TimeDelta, 1.f);

		//// ▣ : 위의 코드 말고, 아래 코드 사용하기
		//m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		//m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta, 1.f);
	}

}

CM_PigWarrior_BEE * CM_PigWarrior_BEE::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_PigWarrior_BEE*		pInstance = new CM_PigWarrior_BEE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_PigWarrior_BEE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_PigWarrior_BEE::Clone(void * pArg)
{
	CM_PigWarrior_BEE*		pInstance = new CM_PigWarrior_BEE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_PigWarrior_BEE");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_PigWarrior_BEE::Free()
{
	__super::Free();


}
