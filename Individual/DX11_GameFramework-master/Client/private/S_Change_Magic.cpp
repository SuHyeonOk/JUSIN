#include "stdafx.h"
#include "..\public\S_Change_Magic.h"

#include "GameInstance.h"
#include "Bone.h"
#include "S_Skill_Weapon.h"
#include "Jake.h"

#include "Obj_Manager.h"
#include "Skill_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

CS_Change_Magic::CS_Change_Magic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Change_Magic::CS_Change_Magic(const CS_Change_Magic & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Change_Magic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Change_Magic::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	m_wsTag = L"Jake";
	m_pPlayer_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));
	m_pPlayer_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
	m_pPlayer_ColliderCom = dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Collider"), 0));

	m_pNavigationCom->Set_CellIndex(m_pPlayer_NavigationCom->Get_CellIndex());	// 현재 플레이어의 네비를 넣어준다. (한 번)

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CS_Change_Magic::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Death_Set(TimeDelta);
	Effect_Create(TimeDelta);

	m_pPlayer_TransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_pPlayer_NavigationCom->Set_CellIndex(m_pNavigationCom->Get_CellIndex());

	KeyInput(TimeDelta);
	Skill_Tick(TimeDelta);

	// 내 무기 콜라이더 공격 중일 때만 On
	if (CSkill_Manager::MAGICSKILL::ATTACK == CSkill_Manager::GetInstance()->Get_Magic_Skill().eSkill)
	{
		m_dAttack_TimeAcc += TimeDelta;
		if (1.6 < m_dAttack_TimeAcc)
		{
			CUI_Manager::GetInstance()->Set_Ui_Monster(false);
			CObj_Manager::GetInstance()->Set_Monster_Crash(false);
			return;
		}

		if (1.4 < m_dAttack_TimeAcc)
			m_SkillParts[0]->Tick(TimeDelta);
	}
	else
		m_dAttack_TimeAcc = 0;
}

void CS_Change_Magic::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);		// 충돌처리
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	//Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);


	if (CSkill_Manager::MAGICSKILL::ATTACK == CSkill_Manager::GetInstance()->Get_Magic_Skill().eSkill)
	{
		m_dAttack_TimeAcc += TimeDelta;
		if (1.6 < m_dAttack_TimeAcc)
			return;

		if (1.4 < m_dAttack_TimeAcc)
			m_SkillParts[0]->Late_Tick(TimeDelta);
	}
	else
		m_dAttack_TimeAcc = 0;
}

HRESULT CS_Change_Magic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

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

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();

		m_pNavigationCom->Render();
	}
#endif

	return S_OK;
}

void CS_Change_Magic::On_Collision(CGameObject * pOther)
{

}

HRESULT CS_Change_Magic::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Magic_Man_Jake"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.0f, 0.7f, 1.0f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Change_Magic::SetUp_ShaderResources()
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

HRESULT CS_Change_Magic::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CS_Skill_Weapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(WeaponDesc));

	WeaponDesc.eWeaponType = CS_Skill_Weapon::WEAPONDESC::JAKE_MAGIC;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Jake_Arm_Mesh");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_S_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_SkillParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CS_Change_Magic::Death_Set(const _double & TimeDelta)
{
	m_dSkillClone_TimeAcc += TimeDelta;

	// 이펙트
	if (21 < m_dSkillClone_TimeAcc)
	{
		m_OnMove = false;

		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		CEffect_Manager::GetInstance()->Effect_Smoke(_float3(f4PlayerPos.x, f4PlayerPos.y + 1.0f, f4PlayerPos.z - 1.0f), _float3(0.8f, 0.5f, 1.0f));
		CEffect_Manager::GetInstance()->Effect_Star3_Create(_float3(f4PlayerPos.x, f4PlayerPos.y + 1.0f, f4PlayerPos.z - 0.8f));
	}

	if (22 < m_dSkillClone_TimeAcc)
	{
		// 죽을때 플레이어 원래 상태로 돌려놓는다.
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
		//CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
		//CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CJake * pGameObject = dynamic_cast<CJake*>(pGameInstance->Get_GameObjectPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Prototype_GameObject_Jake"), 0));
		if (nullptr == pGameObject)
			return E_FAIL;
		
		pGameObject->Set_Change();
		RELEASE_INSTANCE(CGameInstance);

		CGameObject::Set_Dead();

		m_dSkillClone_TimeAcc = 0;
		return S_OK;
	}

	return S_OK;
}

void CS_Change_Magic::Effect_Create(const _double & TimeDelta)
{
	// 이펙트
	if (1 < m_dSkillClone_TimeAcc)
		return;

	m_OnMove = false;

	CEffect_Manager::GetInstance()->Effect_Smoke(_float3(m_f3Pos.x, m_f3Pos.y + 1.0f, m_f3Pos.z - 1.0f), _float3(0.8f, 0.5f, 1.0f));
	CEffect_Manager::GetInstance()->Effect_Star3_Create(_float3(m_f3Pos.x, m_f3Pos.y + 1.0f, m_f3Pos.z - 0.8f));
}

void CS_Change_Magic::Skill_Tick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::STATE::HIT == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::HIT);

	switch (CSkill_Manager::GetInstance()->Get_Magic_Skill().eSkill)
	{
	case Client::CSkill_Manager::MAGICSKILL::IDLE:
		m_pModelCom->Set_AnimIndex(4);
		break;
	case Client::CSkill_Manager::MAGICSKILL::RUN:
		m_pModelCom->Set_AnimIndex(5);
		break;
	case Client::CSkill_Manager::MAGICSKILL::ATTACK:
		m_pModelCom->Set_AnimIndex(1, false);
		Attack_Tick();
		break;
	case Client::CSkill_Manager::MAGICSKILL::HIT:
		m_pModelCom->Set_AnimIndex(3, false);
		Hit_Tick(TimeDelta);
		break;
	}
}

void CS_Change_Magic::Attack_Tick()
{
	CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);

	if (m_pModelCom->Get_Finished())
	{
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
}

void CS_Change_Magic::Hit_Tick(const _double TimeDelta)
{
	m_OnMove = false;

	m_bShader_Hit = true;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (m_pModelCom->Get_Finished())
	{
		m_bShader_Hit = false;
		m_dShader_Hit_TimeAcc = 0;

		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
}

void CS_Change_Magic::KeyInput(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_OnMove)
	{
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::RUN);
	}

	if (m_OnMove && 15 <= m_pModelCom->Get_Keyframes())
		m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
	else if (m_OnMove && 15 >= m_pModelCom->Get_Keyframes())
		m_pTransformCom->Go_Straight(TimeDelta * 0.5, m_pNavigationCom);
		
#pragma region 이동
	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
	}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));

		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_OnMove = true;
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(270.f));

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	}
#pragma endregion

	if (pGameInstance->Key_Up(DIK_UP) || pGameInstance->Key_Up(DIK_RIGHT) || pGameInstance->Key_Up(DIK_DOWN) || pGameInstance->Key_Up(DIK_LEFT))
	{
		m_OnMove = false;
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);
	}

	if (pGameInstance->Key_Down(DIK_SPACE))
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::ATTACK);


	RELEASE_INSTANCE(CGameInstance);
}

CS_Change_Magic * CS_Change_Magic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Change_Magic*		pInstance = new CS_Change_Magic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Change_Magic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Change_Magic::Clone(void * pArg)
{
	CS_Change_Magic*		pInstance = new CS_Change_Magic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Change_Magic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Change_Magic::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
