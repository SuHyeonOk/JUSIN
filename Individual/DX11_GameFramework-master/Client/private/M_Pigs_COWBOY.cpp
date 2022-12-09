#include "stdafx.h"
#include "..\public\M_Pigs_COWBOY.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"

CM_Pigs_COWBOY::CM_Pigs_COWBOY(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Pigs_COWBOY::CM_Pigs_COWBOY(const CM_Pigs_COWBOY & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Pigs_COWBOY::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Pigs_COWBOY::Initialize(void * pArg)
{
	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	MonsterDesc.TransformDesc.fSpeedPerSec = 2.5f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(7);

	m_tMonsterInfo.eState	= m_tMonsterInfo.IDLE;
	m_tMonsterInfo.iHp		= 30;
	m_tMonsterInfo.iExp		= 30;
	m_tMonsterInfo.iAttack	= 10;

	return S_OK;
}

void CM_Pigs_COWBOY::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// ���� �Ѿ��� ��ġ�� ���ϴٰ� �Ѿ��� 

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		// TODO : �浹ó���� ������ ���� ����
		m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Pigs_COWBOY::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Pigs_COWBOY::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����Ѵ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CM_Pigs_COWBOY::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Pigs_COWBOY"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Pigs_COWBOY::SetUp_ShaderResources()
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

void CM_Pigs_COWBOY::Monster_Tick(_double TimeDelta)
{
	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick();
		m_pModelCom->Set_AnimIndex(7);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick();
		m_pModelCom->Set_AnimIndex(9);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick();
		m_pModelCom->Set_AnimIndex(6);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick();
		m_pModelCom->Set_AnimIndex(3);
		break;
	}
}

void CM_Pigs_COWBOY::Idle_Tick()
{
	// IDLE ���·� �ִٰ� �÷��̾ �ֺ��� �ִ��� ��� Ȯ���Ѵ�.

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);			// �� ��ǥ
	_vector		vDir = CObj_Manager::GetInstance()->Get_Player_Transform() - vMyPos;		// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));							// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (fDistanceX < 5.f)	// �Ÿ� �� ���� ���Դٸ� ATTACK, �ٶ󺸱�
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;
		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
	}
}

void CM_Pigs_COWBOY::Move_Tick()
{
}

void CM_Pigs_COWBOY::Attack_Tick(_double TimeDelta)
{
	// ���� ��ġ���� �÷��̾ ���� �Ѿ� �߻�.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	// �Ѿ� ���� �ȿ� ������ 1�ʰ� ������ �Ѿ��� �߻��Ѵ�.
	m_dBullet_TimeAcc += TimeDelta;
	if (2 < m_dBullet_TimeAcc)
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, TEXT("B_Star_0"), TEXT("Prototype_GameObject_B_Star"), &_float3(f4MyPos.x, f4MyPos.y + 0.3f, f4MyPos.z))))
			return;

		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
		m_dBullet_TimeAcc = 0;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CM_Pigs_COWBOY::Hit_Tick()
{
}

void CM_Pigs_COWBOY::Die_Tick()
{
	// ���Ͱ� �װ� ���� �� �ൿ

	if (0 >= m_tMonsterInfo.iHp)
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;

		CObj_Manager::GetInstance()->Set_Player_Exp(m_tMonsterInfo.iExp);	// �÷��̾�� ����ġ

		if (!m_OneCoin)	// ���� ����
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

CM_Pigs_COWBOY * CM_Pigs_COWBOY::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Pigs_COWBOY*		pInstance = new CM_Pigs_COWBOY(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Pigs_COWBOY");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Pigs_COWBOY::Clone(void * pArg)
{
	CM_Pigs_COWBOY*		pInstance = new CM_Pigs_COWBOY(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Pigs_COWBOY");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Pigs_COWBOY::Free()
{
	__super::Free();


}
