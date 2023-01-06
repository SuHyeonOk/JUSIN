#include "stdafx.h"
#include "..\public\M_PigWarrior.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"

#include "UI_3DTexture.h"
#include "W_PigWarrior.h"
#include "Bone.h"

CM_PigWarrior::CM_PigWarrior(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_PigWarrior::CM_PigWarrior(const CM_PigWarrior & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_PigWarrior::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_PigWarrior::Initialize(void * pArg)
{
	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_BEE)
	{
		m_wsTag = L"Monster__PigWarrior_Bee";
		MonsterDesc.TransformDesc.fSpeedPerSec = 1.5f;
		MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
		m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_WORKE)
	{
		m_wsTag = L"Monster__PigWarrior_Worke";
		MonsterDesc.TransformDesc.fSpeedPerSec = 1.5f;
		MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
		MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
		m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);
	}

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
	m_tMonsterInfo.fHP		= 50.0f;
	m_tMonsterInfo.fMaxHP	= 50.0f;
	m_tMonsterInfo.fExp		= 25.0f;
	m_tMonsterInfo.fAttack	= 5.0f;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	return S_OK;
}

void CM_PigWarrior::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_MonsterParts[0]->Tick(TimeDelta);
	m_MonsterParts[1]->Tick(TimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

void CM_PigWarrior::Late_Tick(_double TimeDelta)
{

	m_MonsterParts[0]->Late_Tick(TimeDelta);
	m_MonsterParts[1]->Late_Tick(TimeDelta);

	__super::Late_Tick(TimeDelta);
}

HRESULT CM_PigWarrior::Render()
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
			if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_BEE)
				if (2 == i)	continue;	// �ܹ� Į ���ֱ�
			if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_WORKE)
				if (1 == i) continue;	// �뵿�� Į ���ֱ�

			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

			if (i == 0)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
			else
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);
		}

		return S_OK;	// �׾��ٸ�, ������� �����ϰ� return
	}

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_BEE)
			if (2 == i)	continue;	// �ܹ� Į ���ֱ�
		if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_WORKE)
			if (1 == i) continue;	// �뵿�� Į ���ֱ�

		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 0)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CM_PigWarrior::On_Collision(CGameObject * pOther)
{
	CM_Monster::On_Collision(pOther);

	// TODO : !�ϰ� �ʹٸ�..? PigWarrior �� ��쿡�� ���� ������ ���������� �浹 ó��
}

HRESULT CM_PigWarrior::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_BEE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_BEE"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.W_WORKE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_WORKER"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

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

HRESULT CM_PigWarrior::SetUp_ShaderResources()
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

HRESULT CM_PigWarrior::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CW_PigWarrior::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CW_PigWarrior::WEAPONDESC));

	WeaponDesc.eWarriorType = WeaponDesc.SWORD;
	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("woodenSword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PigWarrior_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);

	WeaponDesc.eWarriorType = WeaponDesc.CYLINDER;
	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone001_shield");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PigWarrior_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CM_PigWarrior::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
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
		m_pModelCom->Set_AnimIndex(3, false);
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
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		CM_Monster::Dance_Time();
		m_pModelCom->Set_AnimIndex(3);
		break;
	}
}

void CM_PigWarrior::Idle_Tick(const _double& TimeDelta)
{
	// IDLE �� ��, MOVE �� �� �Ȱ��� �Ÿ� �̳� �÷��̾ �ִ��� Ȯ���Ѵ�.
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.f > fDistance)	// �� �÷��̾� Ž�� ����		
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	// ���ٸ� IDLE �� MOVE �� ������ ���� �����Ѵ�.
	if(m_pModelCom->Animation_Check(7) && m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_PigWarrior::Move_Tick(const _double& TimeDelta)
{
	// 1 : �÷��̾ ã���� ���� MOVE
	// 2 : �÷��̾ ã�� �� �ϰ� �������� �̵��ϴ� MOVE

	if (m_bFind)		// �÷��̾ ã���� ��! �÷��̾�� �Ÿ��� 1�� �� �� ���� �̵��Ѵ�.
	{
		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta);

		// �Ÿ��� 4 �� �̶�� �÷��̾ ���󰡼� �Ÿ��� 1�� �� �� �����Ѵ�.
		if (1.f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
			m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

		// �� �÷��̾� ���� ����
		if (4.f < CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
			m_bFind = false; // ũ��.. ���� ����� �� �� ��۹�� ���鼭 ȸ���ϴ� ���� ����..

	}
	else				// �÷��̾ ã�� �� ���� �� �������� �̵��ϰ� �ִ´�
	{
		// MOVE �� ��, IDLE �� �� �Ȱ��� �Ÿ� �̳� �÷��̾ �ִ��� Ȯ���Ѵ�.
		_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		if (!m_bAttack && 3.f > fDistance)	// �� �÷��̾� Ž�� ����
			m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

		// �� ���� �Ÿ��� �� ��ġ�� �ִٸ�! ������ �������� ���ư���.
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
				m_bAttack = false;
			}
		}
	}
}

void CM_PigWarrior::Find_Tick()
{
	m_bFind = true;	// �÷��̾ ã�Ҵٸ� �÷��̾�� �ٰ����� ���ؼ� MOVE�� �̵��Ѵ�.

	if (25 == m_pModelCom->Get_Keyframes())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	// 3D UI �� ����ǥ�� ����ֱ� ���ؼ� �ۼ��� �ڵ�
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector	vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CUI_3DTexture::TEXTUREINFO	tTextureInfo;
	tTextureInfo.eTextureType = tTextureInfo.TYPE_FIND;
	tTextureInfo.f2Size = _float2(0.7f, 0.7f);
	tTextureInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z - 0.5f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_UI_Find_0"), TEXT("Prototype_GameObject_UI_3DTexture"), &tTextureInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CM_PigWarrior::Attack_Tick(const _double& TimeDelta)
{
	m_bFind = false;

	_int	iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(0, 1);

	if (0 == iRandomNum && m_pModelCom->Get_Finished())	// �������� 0�� ������ �ٷ� MOVE�� ����, 1�� ���� ATTACK �̴�.
	{
		m_bAttack = true;
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
	}
}

void CM_PigWarrior::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_PigWarrior::Die_Tick(const _double& TimeDelta)
{
	CM_Monster::Die(TimeDelta, 1.0f, 5, 1, 0);
}

CM_PigWarrior * CM_PigWarrior::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_PigWarrior*		pInstance = new CM_PigWarrior(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_PigWarrior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_PigWarrior::Clone(void * pArg)
{
	CM_PigWarrior*		pInstance = new CM_PigWarrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_PigWarrior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_PigWarrior::Free()
{
	__super::Free();

	for (auto& pPart : m_MonsterParts)
		Safe_Release(pPart);
	m_MonsterParts.clear();
}