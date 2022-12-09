#include "stdafx.h"
#include "..\public\M_PigWarrior_WORKER.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"

CM_PigWarrior_WORKER::CM_PigWarrior_WORKER(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_PigWarrior_WORKER::CM_PigWarrior_WORKER(const CM_PigWarrior_WORKER & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_PigWarrior_WORKER::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior_WORKER::Initialize(void * pArg)
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
	m_tMonsterInfo.iHp		= 50;
	m_tMonsterInfo.iExp			= 25;
	m_tMonsterInfo.iAttack	= 5;

	return S_OK;
}

void CM_PigWarrior_WORKER::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Die();

	ToThe_Player(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		// TODO : �浹ó���� ������ ���� ����
		m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CM_PigWarrior_WORKER::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_PigWarrior_WORKER::Render()
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

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CM_PigWarrior_WORKER::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_WORKER"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior_WORKER::SetUp_ShaderResources()
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

void CM_PigWarrior_WORKER::Monster_Die()
{
	// ���Ͱ� �װ� ���� �� �ൿ

	if (0 >= m_tMonsterInfo.iHp)
	{
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

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

void CM_PigWarrior_WORKER::ToThe_Player(const _double & TimeDelta)
{
	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);			// �� ��ǥ
	_vector		vDir = CObj_Manager::GetInstance()->Get_Player_Transform() - vMyPos;		// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (fDistanceX < 5.f)
	{
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_pTransformCom->Chase(XMVectorSet(f4PlayerPos.x, f4PlayerPos.y, f4PlayerPos.z, 1.f), TimeDelta, 1.f);

		// �� : ���� �ڵ� ����, �Ʒ� �ڵ� ����ϱ�
		//m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta, 11.f5);
	}

}

CM_PigWarrior_WORKER * CM_PigWarrior_WORKER::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_PigWarrior_WORKER*		pInstance = new CM_PigWarrior_WORKER(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_PigWarrior_WORKER");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_PigWarrior_WORKER::Clone(void * pArg)
{
	CM_PigWarrior_WORKER*		pInstance = new CM_PigWarrior_WORKER(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_PigWarrior_WORKER");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_PigWarrior_WORKER::Free()
{
	__super::Free();


}
