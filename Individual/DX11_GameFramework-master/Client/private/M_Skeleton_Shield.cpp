#include "stdafx.h"
#include "..\public\M_Skeleton_Shield.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "ItemManager.h"
#include "Utilities_Manager.h"
#include "UI_Manager.h"

#include "W_Skeleton_Shield.h"
#include "Bone.h"

CM_Skeleton_Shield::CM_Skeleton_Shield(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CM_Monster(pDevice, pContext)
{

}

CM_Skeleton_Shield::CM_Skeleton_Shield(const CM_Skeleton_Shield & rhs)
	: CM_Monster(rhs)
{

}

HRESULT CM_Skeleton_Shield::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Skeleton_Shield::Initialize(void * pArg)
{
	m_wsTag = L"Monster__Skeleton_Shield";

	CM_Monster::MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MonsterDesc));

	m_tMonsterDesc.eMonsterKind = MonsterDesc.eMonsterKind;

	MonsterDesc.TransformDesc.fSpeedPerSec = 2.f;
	MonsterDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MonsterDesc.TransformDesc.f3Pos = _float3(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z);
	m_f4CenterPos = _float4(MonsterDesc.f3Pos.x, MonsterDesc.f3Pos.y, MonsterDesc.f3Pos.z, 1.f);

	if (FAILED(CM_Monster::Initialize(&MonsterDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tMonsterInfo.eState	= m_tMonsterInfo.MOVE;
	m_tMonsterInfo.fHP		= 150.0f;
	m_tMonsterInfo.fMaxHP	= m_tMonsterInfo.fHP;
	m_tMonsterInfo.fExp		= 15.0f;
	m_tMonsterInfo.fAttack	= 30.0f;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	return S_OK;
}

void CM_Skeleton_Shield::Tick(_double TimeDelta)
{
	//__super::Tick(TimeDelta);

	BossCage();

	Monster_Tick(TimeDelta);
	Hit_Process(TimeDelta);

	if (m_tMonsterInfo.ATTACK == m_tMonsterInfo.eState)
		m_MonsterParts[0]->Tick(TimeDelta);
	m_MonsterParts[1]->Tick(TimeDelta);
}

void CM_Skeleton_Shield::Late_Tick(_double TimeDelta)
{
	if(m_tMonsterInfo.ATTACK == m_tMonsterInfo.eState)
		m_MonsterParts[0]->Late_Tick(TimeDelta);
	m_MonsterParts[1]->Late_Tick(TimeDelta);

	__super::Late_Tick(TimeDelta);

	// ���� �ް� ���� ��, ���� ��, ������ �ȿ� ���� �� Xray �� ȣ������ �ʴ´�.
	if (true == m_bShader_Hit || 1 != m_fAlpha || true == CObj_Manager::GetInstance()->Get_BossCage())
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_XRAYBLEND, this);
	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CM_Skeleton_Shield::Render()
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

		return S_OK;	// �׾��ٸ�, ������� �����ϰ� return
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

HRESULT CM_Skeleton_Shield::Render_XRay()
{
	if (FAILED(__super::Render_XRay()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderXRayResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (0 == i || 1 == i)
			continue;

		if (m_bShader_Hit)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
		else
			m_pModelCom->Render(m_pShaderXRayCom, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CM_Skeleton_Shield::On_Collision(CGameObject * pOther)
{
	// �� ������ ��쿡�� ���и� ��� �ֱ� ������ ���� ó���ߴ�.
	if (CObj_Manager::PLAYERINFO::STATE::ATTACK == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		if (L"Player_Weapon" == pOther->Get_Tag())
			m_tMonsterInfo.eState = m_tMonsterInfo.HIT;
	}

	if (L"Skill_Paint" == pOther->Get_Tag())
		m_tMonsterInfo.eState = m_tMonsterInfo.HIT;

	if (L"Skill_Marceline" == pOther->Get_Tag())
	{
		m_bDance = true;
		m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::DANCE;
	}
}

HRESULT CM_Skeleton_Shield::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_ShaderXRay */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel_XRay"), TEXT("Com_ShaderXRay"),
		(CComponent**)&m_pShaderXRayCom)))
		return E_FAIL;

	if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_SHIELD_1)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_2"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tMonsterDesc.eMonsterKind == m_tMonsterDesc.SKELETON_SHIELD_2)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_4"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 1.2f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CM_Skeleton_Shield::SetUp_ShaderResources()
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

HRESULT CM_Skeleton_Shield::SetUp_ShaderXRayResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderXRayCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float	fObjectID = 3.0f;
	if (FAILED(m_pShaderXRayCom->Set_RawValue("g_ObjectID", &fObjectID, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Skeleton_Shield::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CW_Skeleton_Shield::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CW_Skeleton_Shield::WEAPONDESC));

	WeaponDesc.eWarriorType = WeaponDesc.SWORD;
	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_sword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_W_Skeleton_Shield"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);

	WeaponDesc.eWarriorType = WeaponDesc.SHIELD;
	WeaponDesc.fAttack = m_tMonsterInfo.fAttack;
	WeaponDesc.fExp = m_tMonsterInfo.fExp;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_shield");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_W_Skeleton_Shield"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CM_Skeleton_Shield::Monster_Tick(const _double& TimeDelta)
{
	if (0.0f >= m_tMonsterInfo.fHP)
		m_tMonsterInfo.eState = m_tMonsterInfo.DIE;

	// 0 : ATTACK
	// 1 : �ݴ�� ����
	// 2 : HIT
	// 3 : ���и���
	// 4 : DIE
	// 5 : ����ä�� ������
	// 6 : ��Ƴ�
	// 7 : ��
	// 8 : �� ���ư�
	// 9 : HIT
	// 10 : �Ȱ��� ��Ʈ
	// 11 : IDLE
	// 12 : MOVE

	switch (m_tMonsterInfo.eState)
	{
	case MONSTERINFO::STATE::IDLE:
		Idle_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(11, false);
		break;

	case MONSTERINFO::STATE::MOVE:
		Move_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(12, false);
		break;

	case MONSTERINFO::STATE::FIND:
		Find_Tick();
		m_pModelCom->Set_AnimIndex(8, false);
		break;

	case MONSTERINFO::STATE::ATTACK:
		Attack_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(0, false);
		break;

	case MONSTERINFO::STATE::HIT:
		Hit_Tick(TimeDelta);
		break;

	case MONSTERINFO::STATE::DIE:
		Die_Tick(TimeDelta);
		m_pModelCom->Set_AnimIndex(4, false);
		break;

	case MONSTERINFO::STATE::DANCE:
		Dance_Tick();
		m_pModelCom->Set_AnimIndex(7);
		break;
	}
}

void CM_Skeleton_Shield::Idle_Tick(const _double& TimeDelta)
{
	m_bIdle_Sound = false;

	// IDLE �� ��, MOVE �� �� �Ȱ��� �Ÿ� �̳� �÷��̾ �ִ��� Ȯ���Ѵ�.
	_float	fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	if (!m_bAttack && 3.5f > fDistance)	// �� �÷��̾� Ž�� ����		
		m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

	// ���ٸ� IDLE �� MOVE �� ������ ���� �����Ѵ�.
	if(m_pModelCom->Animation_Check(11) && m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
}

void CM_Skeleton_Shield::Move_Tick(const _double& TimeDelta)
{
	_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (7.0f > fDistance)
	{
		if (false == m_bIdle_Sound)
		{
			m_bIdle_Sound = true;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("Skeleton_Idle.mp3"), 0.7f);
			RELEASE_INSTANCE(CGameInstance);
		}
	}

	// 1 : �÷��̾ ã���� ���� MOVE
	// 2 : �÷��̾ ã�� �� �ϰ� �������� �̵��ϴ� MOVE

	if (m_bFind)		// �÷��̾ ã���� ��! �÷��̾�� �Ÿ��� 1�� �� �� ���� �̵��Ѵ�.
	{
		m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta);

		// �Ÿ��� 4 �� �̶�� �÷��̾ ���󰡼� �Ÿ��� 1�� �� �� �����Ѵ�.
		if (2.f > fDistance)
			m_tMonsterInfo.eState = m_tMonsterInfo.ATTACK;

		// �� �÷��̾� ���� ����
		if (4.f < fDistance)
			m_bFind = false; // ũ��.. ���� ����� �� �� ��۹�� ���鼭 ȸ���ϴ� ���� ����..

	}
	else				// �÷��̾ ã�� �� ���� �� �������� �̵��ϰ� �ִ´�
	{
		// MOVE �� ��, IDLE �� �� �Ȱ��� �Ÿ� �̳� �÷��̾ �ִ��� Ȯ���Ѵ�.
		if (!m_bAttack && 3.5f > fDistance)	// �� �÷��̾� Ž�� ����
			m_tMonsterInfo.eState = m_tMonsterInfo.FIND;

		// �� ���� �Ÿ��� �� ��ġ�� �ִٸ�! ������ �������� ���ư���.
		_vector	vCenterPos = XMLoadFloat4(&m_f4CenterPos);
		_vector vDistance = vCenterPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float	fStartDiatance = XMVectorGetX(XMVector3Length(vDistance));

		if (2.1f < fStartDiatance)
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

void CM_Skeleton_Shield::Find_Tick()
{
	m_bIdle_Sound = false;
	m_bFind = true;	// �÷��̾ ã�Ҵٸ� �÷��̾�� �ٰ����� ���ؼ� MOVE�� �̵��Ѵ�.

	if(m_pModelCom->Get_Finished())
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;

	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());
}

void CM_Skeleton_Shield::Attack_Tick(const _double& TimeDelta)
{
	m_bIdle_Sound = false;

	if (false == m_bSword_Sound)
	{
		m_bSword_Sound = true;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_Sound(TEXT("sfx_enchiridion_throwing_pie.ogg"), 1.0f);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_bFind = false;

	if (m_pModelCom->Get_Finished())
	{
		m_bSword_Sound = false;
		m_bAttack = true;
		m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
	}
}

void CM_Skeleton_Shield::Hit_Tick(const _double& TimeDelta)
{
	if (0 == m_iRandomNum)
		m_iRandomNum = CUtilities_Manager::GetInstance()->Get_Random(1, 2);
	
	if (true == m_bDance)		// �������� ������ ����� ���� ������ ������ �޴´�.
		m_iRandomNum = 1;

	if (1 == m_iRandomNum)
	{
		m_bPlayer_Attack = true;
		CM_Monster::Effect_Hit();
		m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.2f);

		if (false == m_bDance)
		{
			m_pModelCom->Set_AnimIndex(2, false);	// Hit
			if (m_pModelCom->Get_Finished())
			{
				m_iRandomNum = 0;
				m_bShader_Hit = false;

				m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::ATTACK;
			}
		}
		else
		{
			m_iRandomNum = 0;
			m_bShader_Hit = false;

			m_tMonsterInfo.eState = CM_Monster::MONSTERINFO::STATE::IDLE;
		}
	}
	else if (2 == m_iRandomNum)
	{
		if (false == m_bShield_Sound)
		{
			m_bShield_Sound = true;
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("Skeleton_Shield.mp3"), 1.0f);
			RELEASE_INSTANCE(CGameInstance);

		}

		m_pModelCom->Set_AnimIndex(3, false);	// ���
		if (m_pModelCom->Get_Finished())
		{
			m_bShield_Sound = false;
			m_iRandomNum = 0;
			
			m_tMonsterInfo.eState = m_tMonsterInfo.MOVE;
		}
	}
}

void CM_Skeleton_Shield::Die_Tick(const _double& TimeDelta)
{
	CM_Monster::Die(TimeDelta, 1.1f, 15, 6, 6);
}

void CM_Skeleton_Shield::Dance_Tick()
{
	if (CObj_Manager::PLAYERINFO::STATE::S_MARCELINE != CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		m_bDance = false;
		m_tMonsterInfo.eState = m_tMonsterInfo.IDLE;
	}
}

void CM_Skeleton_Shield::Hit_Process(const _double & TimeDelta)
{
	if (!m_bPlayer_Attack)		// �÷��̾�� �浹 ���� ��
		return;

	// ���� ���� �޴� ��...

	// ���� Hit ���̴� ��� ����!
	m_bShader_Hit = true;

	if (0.1 < m_dPlayer_Attack_TimeAcc)
		m_bShader_Hit = false;

	// �� ó�� �� �� ü���� ��´�.
	if (0 == m_dPlayer_Attack_TimeAcc)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Play_Sound(TEXT("sfx_character_hit_1.ogg"), 0.7f);
		RELEASE_INSTANCE(CGameInstance);

		if (false == m_bDance)
			m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());				// �÷��̾ �ٶ󺸸鼭
		m_pTransformCom->Go_Backward(_float(TimeDelta) * 0.05f);										// ���� �˹�

		m_tMonsterInfo.fHP -= CObj_Manager::GetInstance()->Get_Player_Attack();							// �÷��̾��� ���ݷ� ���� ���� ü�� ���
		CUI_Manager::GetInstance()->Set_HPGauge_Monster(m_tMonsterInfo.fHP / m_tMonsterInfo.fMaxHP);	// UI �� �� ü�� �Ѱ��ֱ�
	}
	// ���� ���� ���� (�� �ϸ� ��� ���� ����)
	m_dPlayer_Attack_TimeAcc += TimeDelta;
	if (0.7 < m_dPlayer_Attack_TimeAcc)
	{
		m_bPlayer_Attack = false;
		m_dPlayer_Attack_TimeAcc = 0;
		return;
	}
}

void CM_Skeleton_Shield::BossCage()
{
	if (false == CObj_Manager::GetInstance()->Get_BossCage())
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4Pos, vPos);

	if (1.0f > f4Pos.x || 9.5f < f4Pos.x || 14.0f > f4Pos.z || 18.0f < f4Pos.z)
	{
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook *= -1.0f;
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
}

CM_Skeleton_Shield * CM_Skeleton_Shield::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Skeleton_Shield*		pInstance = new CM_Skeleton_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Skeleton_Shield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Skeleton_Shield::Clone(void * pArg)
{
	CM_Skeleton_Shield*		pInstance = new CM_Skeleton_Shield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Skeleton_Shield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Skeleton_Shield::Free()
{
	__super::Free();

	for (auto& pPart : m_MonsterParts)
		Safe_Release(pPart);
	m_MonsterParts.clear();

	Safe_Release(m_pShaderXRayCom);
}