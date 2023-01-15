#include "stdafx.h"
#include "..\public\S_Fiona.h"

#include "GameInstance.h"
#include "Bone.h"
#include "S_Skill_Weapon.h"

#include "Obj_Manager.h"
#include "Skill_Manager.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

#include "S_StunChick.h"

CS_Fiona::CS_Fiona(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Fiona::CS_Fiona(const CS_Fiona & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Fiona::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Fiona::Initialize(void * pArg)
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

	CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);

	// FIONE 일 때는 플레이어의 평소 공격력의 1.5배가 된다.
	m_fOriginal_Player_Attack = CObj_Manager::GetInstance()->Get_Current_Player().fAttack;
	m_fSword_Attack = CObj_Manager::GetInstance()->Get_Current_Player().fAttack * 1.f;
	m_fCat_Attack = CObj_Manager::GetInstance()->Get_Current_Player().fAttack * 2.f;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (CObj_Manager::PLAYERINFO::PLAYER::FINN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		m_wsTag = L"Finn";
		// 현재 플레이어의 네비 인덱스를 받아온다.
		m_pPlayer_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));
		// 현재 플레이어의 트랜스폼을 가져온다. 그리고 스킬의 좌표를 계속 넣어준다.
		m_pPlayer_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));
		// 현재 플레이어의 콜라이더를 가져와서 충돌이 가능하도록 만든다.
		m_pPlayer_ColliderCom = dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Collider"), 0));
	}
	else	// Jake
	{
		m_wsTag = L"Jake";
		m_pPlayer_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));
		m_pPlayer_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
		m_pPlayer_ColliderCom = dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Collider"), 0));
	}
	
	m_pNavigationCom->Set_CellIndex(m_pPlayer_NavigationCom->Get_CellIndex());	// 현재 플레이어의 네비를 넣어준다. (한 번)

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CS_Fiona::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 예외 처리로, 스킬이 끝나면 없앤다.
	if(CSkill_Manager::PLAYERSKILL::SKILL_END == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		CGameObject::Set_Dead();

	// 죽을 때의 처리
	Death_Set(TimeDelta);
	Effect_Create(TimeDelta);
	
	// 실행중 실제 플레이어의 좌표 옮기기
	m_pPlayer_TransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_pPlayer_NavigationCom->Set_CellIndex(m_pNavigationCom->Get_CellIndex());

	KeyInput(TimeDelta);
	Skill_Tick(TimeDelta);

	Parts_Tick(TimeDelta);
}

void CS_Fiona::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Parts_LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);		// 충돌처리
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CS_Fiona::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (4 == i)
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

void CS_Fiona::On_Collision(CGameObject * pOther)
{

}

HRESULT CS_Fiona::SetUp_Components()
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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Fiona"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.4f, 1.2f, 0.4f);
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

HRESULT CS_Fiona::SetUp_ShaderResources()
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

HRESULT CS_Fiona::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CS_Skill_Weapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(WeaponDesc));

	WeaponDesc.eWeaponType = CS_Skill_Weapon::WEAPONDESC::FIONA_SWORD;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Sword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_S_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_SkillParts.push_back(pPartObject);

	WeaponDesc.eWeaponType = CS_Skill_Weapon::WEAPONDESC::FIONA_CAT;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("morningstar");
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

void CS_Fiona::Parts_Tick(const _double & TimeDelta)
{
	if (CSkill_Manager::FIONASKILL::ATTACK == CSkill_Manager::GetInstance()->Get_Fiona_Skill().eSkill)
		m_SkillParts[0]->Tick(TimeDelta);

	if (CSkill_Manager::FIONASKILL::CAT == CSkill_Manager::GetInstance()->Get_Fiona_Skill().eSkill)
	{
		if (1.7 < m_dCat_Attack_TimeAcc)
			return;

		m_dCat_Attack_TimeAcc += TimeDelta;
		if (1.5 < m_dCat_Attack_TimeAcc)
			m_SkillParts[1]->Tick(TimeDelta);
	}
	else
		m_dCat_Attack_TimeAcc = 0;
}

void CS_Fiona::Parts_LateTick(const _double & TimeDelta)
{
	if (CSkill_Manager::FIONASKILL::ATTACK == CSkill_Manager::GetInstance()->Get_Fiona_Skill().eSkill)
		m_SkillParts[0]->Late_Tick(TimeDelta);

	if (CSkill_Manager::FIONASKILL::CAT == CSkill_Manager::GetInstance()->Get_Fiona_Skill().eSkill)
	{
		if (1.7 < m_dCat_Attack_TimeAcc)
			return;

		m_dCat_Attack_TimeAcc += TimeDelta;
		if (1.5 < m_dCat_Attack_TimeAcc)
			m_SkillParts[1]->Late_Tick(TimeDelta);
	}
	else
		m_dCat_Attack_TimeAcc = 0;
}

void CS_Fiona::Death_Set(const _double & TimeDelta)
{
	// 죽을 때의 처리
	m_bSkillClone_TimeAcc += TimeDelta;

	if (21 < m_bSkillClone_TimeAcc)
	{
		m_OnMove = false;

		// 따라오던 플레이어의 좌표를 옮겨놓는다.
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform * pFollow_TransformCom;
		CNavigation* pFollow_NavigationCom;

		if (CObj_Manager::PLAYERINFO::PLAYER::FINN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		{
			pFollow_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
			pFollow_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));
		}
		else
		{
			pFollow_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));
			pFollow_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));
		}

		pFollow_TransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) * 0.99f);
		pFollow_NavigationCom->Set_CellIndex(m_pNavigationCom->Get_CellIndex());

		RELEASE_INSTANCE(CGameInstance);

		// 위에서 좌표를 옮겨 두고, 이펙트 처리
		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		CEffect_Manager::GetInstance()->Effect_Smoke(_float3(f4PlayerPos.x, f4PlayerPos.y + 1.0f, f4PlayerPos.z - 0.7f), _float3(0.8f, 0.7f, 0.8f));
		CEffect_Manager::GetInstance()->Effect_Star3_Create(_float3(f4PlayerPos.x, f4PlayerPos.y + 1.0f, f4PlayerPos.z - 0.8f));
	}

	if (22 < m_bSkillClone_TimeAcc)
	{
		// 죽으면서 모든 처리를 원래 플레이어로 돌아올 수 있도록 처리한다.
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
		CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
		CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(false);

		CObj_Manager::GetInstance()->Set_Player_Attack(m_fOriginal_Player_Attack);	// 원래의 공격력으로 돌려놓는다.
		CGameObject::Set_Dead();

		m_bSkillClone_TimeAcc = 0;
	}
}

void CS_Fiona::Effect_Create(const _double & TimeDelta)
{
	// 이펙트
	if (1 < m_bSkillClone_TimeAcc)
		return;

	m_OnMove = false;

	// 계속 생성
	CEffect_Manager::GetInstance()->Effect_Smoke(_float3(m_f3Pos.x, m_f3Pos.y + 1.0f, m_f3Pos.z - 0.7f), _float3(0.8f, 0.7f, 0.8f));
	CEffect_Manager::GetInstance()->Effect_Star3_Create(_float3(m_f3Pos.x, m_f3Pos.y + 1.0f, m_f3Pos.z - 0.8f));
}

void CS_Fiona::Skill_Tick(const _double & TimeDelta)
{
	// 0 : 고양이 공격 왼쪽
	// 1 : 고양이 공격 오른쪽
	// 2 : 고양이 돌면서 공격 `
	// 3 : IDLE
	// 4 : MOVE
	// 5 : STUN
	// 6 : DANCE
	// 7 : 넘어짐
	// 8 : ATTACK 반대
	// 9 : 좌우로 이동
	// 10 : 깔아뭉개짐
	// 11 : 뒤로 떨어질랑
	// 12 : 점프
	// 13 : 칼 회오리 공격
	// 14 : ATTACK `
	// 17 : 수영 전
	// 18 : 잡고 들려짐
	// 19 : ATTACK ` 부드러운
	// 21 : HIT
	// 24 : 발랑발랑 댄스

	if (CObj_Manager::PLAYERINFO::STATE::HIT == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::HIT);

	if (CObj_Manager::PLAYERINFO::STATE::STUN == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::STUN);

	switch (CSkill_Manager::GetInstance()->Get_Fiona_Skill().eSkill)
	{
	case Client::CSkill_Manager::FIONASKILL::IDLE:
		m_pModelCom->Set_AnimIndex(3, true, false);
		break;
	case Client::CSkill_Manager::FIONASKILL::RUN:
		m_pModelCom->Set_AnimIndex(4, true, false);
		break;
	case Client::CSkill_Manager::FIONASKILL::ATTACK:
		m_pModelCom->Set_AnimIndex(19, false, false);
		Attack_Tick();
		break;
	case Client::CSkill_Manager::FIONASKILL::CAT:
		m_pModelCom->Set_AnimIndex(2, false, false);
		Cat_Tick();
		break;
	case Client::CSkill_Manager::FIONASKILL::HIT:
		m_pModelCom->Set_AnimIndex(21, false, false);
		Hit_Tick(TimeDelta);
		break;
	case Client::CSkill_Manager::FIONASKILL::STUN:
		m_pModelCom->Set_AnimIndex(5, false);
		Stun_Tick();
		break;
	case Client::CSkill_Manager::FIONASKILL::DANCE://
		m_pModelCom->Set_AnimIndex(24, false);
		Dance_Tick();
		break;
	}
}

void CS_Fiona::Attack_Tick()
{
	CObj_Manager::GetInstance()->Set_Player_Attack(m_fSword_Attack);
	CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);

	if (m_pModelCom->Get_Finished())
	{
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
}

void CS_Fiona::Cat_Tick()
{
	CObj_Manager::GetInstance()->Set_Player_Attack(m_fCat_Attack);
	CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);

	if (m_pModelCom->Get_Finished())
	{
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
}

void CS_Fiona::Hit_Tick(const _double & TimeDelta)
{
	m_bShader_Hit = true;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (21 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	{
		m_bShader_Hit = false;
		m_dShader_Hit_TimeAcc = 0;

		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CS_Fiona::Stun_Tick()
{
	if (!m_bStun)
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("S_StunChick_1"), TEXT("Prototype_GameObject_S_StunChick"), &_float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		m_bStun = true;
	}

	if (m_pModelCom->Get_Finished())
		++m_iStun_Count;

	if (2 <= m_iStun_Count)		// 애니메이션 두 번 재생 후 끝
	{
		m_bStun = false;
		m_iStun_Count = 0;
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CS_Fiona::Dance_Tick()
{
	if (m_pModelCom->Get_Finished())
	{
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
}

void CS_Fiona::KeyInput(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_OnMove)
	{
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::RUN);
		m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
	}
		
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
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::IDLE);
	}

	if (pGameInstance->Key_Down(DIK_SPACE))
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::ATTACK);

	if (pGameInstance->Key_Down(DIK_LCONTROL))
		CSkill_Manager::GetInstance()->Set_Fiona_Skill(CSkill_Manager::FIONASKILL::CAT);

	RELEASE_INSTANCE(CGameInstance);
}

CS_Fiona * CS_Fiona::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Fiona*		pInstance = new CS_Fiona(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Fiona");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Fiona::Clone(void * pArg)
{
	CS_Fiona*		pInstance = new CS_Fiona(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Fiona");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Fiona::Free()
{
	__super::Free();

	for (auto& pPart : m_SkillParts)
		Safe_Release(pPart);
	m_SkillParts.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
