#include "stdafx.h"
#include "..\public\Jake.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Jake_Weapon.h"

#include "ItemManager.h"
#include "Skill_Manager.h"
#include "Effect_Manager.h"

#include "S_PaintWork.h"
#include "S_Fiona.h"
#include "Player_Talk.h"

CJake::CJake(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CJake::CJake(const CJake & rhs)
	: CGameObject(rhs)
{

}

HRESULT CJake::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CJake::Initialize(void * pArg)
{
	m_wsTag = L"Jake";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_tPlayerInfo.ePlayer = m_tPlayerInfo.JAKE;
	m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(18);
	m_pNavigationCom->Set_CellIndex(346);
	m_pTransformCom->Rotation(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(180.f));

	//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	//_vector vUp = XMVector3Cross(vLook, vRight);

	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	return S_OK;
}

void CJake::Tick(_double TimeDelta)
{
	if (true == CSkill_Manager::GetInstance()->Get_ChangeSkill_Create() ||
		m_bChange)
		return;
	
	__super::Tick(TimeDelta);

	BossCage(TimeDelta);
	Parts_Tick(TimeDelta);

	Current_Player(TimeDelta);
	Player_Tick(TimeDelta);
}

void CJake::Late_Tick(_double TimeDelta)
{
	if (true == CSkill_Manager::GetInstance()->Get_ChangeSkill_Create() ||
		m_bChange)
		return;

	__super::Late_Tick(TimeDelta);

	Parts_LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (1 != m_fAlpha)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		else
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_XRAYBLEND, this);
		}
	}

	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

HRESULT CJake::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (2 == i)		// 방패
			continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 4)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
		else
			if (m_bShader_Hit && CObj_Manager::PLAYERINFO::STATE::HIT == m_tPlayerInfo.eState)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);
			else
				if (1 != m_fAlpha)
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);	// 죽을 때
				else
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");		// 평소
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

HRESULT CJake::Render_XRay()
{
	if (m_bShader_Hit && CObj_Manager::PLAYERINFO::STATE::HIT == m_tPlayerInfo.eState || 1 != m_fAlpha)
		return S_OK;

	if (FAILED(__super::Render_XRay()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderXRayResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (2 == i || 4 == i)		// 방패
			continue;

		m_pModelCom->Bind_Material(m_pShaderXRayCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderXRayCom, i, "g_BoneMatrices");	// 평소
	}
	return S_OK;
}

void CJake::On_Collision(CGameObject * pOther)
{
	//CSkill_Manager::GetInstance()->Page_PickUp(pOther);
}

HRESULT CJake::SetUp_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_Jake"), TEXT("Com_Model"),
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

HRESULT CJake::SetUp_ShaderResources()
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

	if (1 != m_fAlpha)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CJake::SetUp_ShaderXRayResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderXRayCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float	fObjectID = 2.0f;
	if (FAILED(m_pShaderXRayCom->Set_RawValue("g_ObjectID", &fObjectID, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJake::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CJake_Weapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CJake_Weapon::WEAPONDESC));

	WeaponDesc.eWeaponType = CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bip001 L Finger0");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Jake_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	WeaponDesc.eWeaponType = CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("R_Arm_Drill");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Jake_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	WeaponDesc.eWeaponType = CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bip001 L Finger0");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Jake_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CJake::Parts_Tick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon ||
		CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
	{
		m_PlayerParts[0]->Tick(TimeDelta);
		m_PlayerParts[1]->Tick(TimeDelta);
		((CJake_Weapon*)m_PlayerParts[2])->ResetJakeS();
		CObj_Manager::GetInstance()->Set_Jake_Shield(false);
	}
	else if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
		m_PlayerParts[2]->Tick(TimeDelta);
}

void CJake::Parts_LateTick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon ||
		CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
	{
		m_PlayerParts[0]->Late_Tick(TimeDelta);
		m_PlayerParts[1]->Late_Tick(TimeDelta);
	}
	else if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
		m_PlayerParts[2]->Late_Tick(TimeDelta);
}

void CJake::Player_Tick(_double TimeDelta)
{
	// Player 가 아닐 때 계속 확인해야하는 기능
	Change_Tick();
	Cheering_Tick();

	// 수영!!
	if (m_bIsSwim)
	{
		Swim_Tick(TimeDelta);
		return;
	}
	else
		if (1 == m_pNavigationCom->Get_CellType())
			m_bIsSwim = true;

	// 예외적으로 Jake의 경우 항시 체크한다.
	JakeSon_Transform_Change();

	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		m_tPlayerInfo.eState = CObj_Manager::GetInstance()->Get_Current_Player().eState;

	switch (m_tPlayerInfo.eState)
	{
	case CObj_Manager::PLAYERINFO::ATTACK:
		Space_Attack_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::S_PAINT: // 11
		Attack_Paint_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::S_MARCELINE: // 12
		Skill_Marceline_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::S_COIN:	// 13
		Skill_Coin_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::S_FOOD:	// 14
		Skill_Food_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::S_FIONA:	// 15
		Skill_Fiona_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::CONTROL:
		Control_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::ROLL:
		Roolling_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::HIT:
		Hit_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::KNOCKBACKHIT:
		KnockBack_Hit_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::STUN:
		Stun_Tick();
		break;

	case CObj_Manager::PLAYERINFO::MAGIC:
		Magic_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::JAKESON:
		JakeSon_Tick(TimeDelta);
		break;
	}

	Anim_Change(TimeDelta);
}

void CJake::Current_Player(_double TimeDelta)
{
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		CObj_Manager::GetInstance()->Tick_Player_Transform();
		Player_Skill_Tick(TimeDelta);
		Current_HP(TimeDelta);

		if (1 != m_fAlpha)	// 부활 해야 하는 시간을 줘야한다.
			return;
		Key_Input(TimeDelta);
	}
	else
	{
		if (false == m_bFinn_Meet)
		{
			m_tPlayerInfo.eState = CObj_Manager::PLAYERINFO::STATE::IDLE;
			m_pTransformCom->LookAt(XMVectorSetY(CObj_Manager::GetInstance()->Get_Player_Transform(), 0.0f));

			if (1.5f > CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)))
			{
				m_bFinn_Meet = true;
				
				// 이펙트
				_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float4 f4MyPos;
				XMStoreFloat4(&f4MyPos, vMyPos);

				CEffect_Manager::GetInstance()->Effect_Star_Create(_float3(f4MyPos.x - 0.5f, f4MyPos.y + 1.0f, f4MyPos.z - 1.0f), 40);
				CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4MyPos.x - 0.5f, f4MyPos.y + 1.0f, f4MyPos.z - 1.1f), _float3(1.0f, 1.0f, 1.0f), 10);
			}

			return;
		}
		else		// 플레이어와 핀이 만난 순간 부터 제이크를 플레이 할 수 있다.
		{
			Talk(TimeDelta);
			Player_Follow(TimeDelta);
			Check_Follow(TimeDelta);
		}
	}
}

void CJake::Player_Skill_Tick(_double TimeDelta)
{
	if (m_bIsSwim)		// 수영 중 에는 스킬 사용 금지
		return;

	// 전체적으로 스킬을 on 한다.
	if (CSkill_Manager::PLAYERSKILL::PAINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill ||
		CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		m_bSkill = true;

	if (CSkill_Manager::PLAYERSKILL::COIN == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill ||
		CSkill_Manager::PLAYERSKILL::FOOD == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		m_bOneSkillStart = true;

	if (m_bSkill)		// 20 초 동안 실행될 스킬의 시간을 계산한다.
	{
		m_dSkill_TimeAcc += TimeDelta;	// 스킬 사용 후 일정시간 뒤 초기화
		if (20 < m_dSkill_TimeAcc)
		{
			// 모든 스킬을 false 로 변경한다. (예외적으로 키 입력을 하는 경우는 추가 처리)
			m_bSkill_Clone = false;

			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
			CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
			m_bSkill = false;
			m_dSkill_TimeAcc = 0;
		}
	}

	if (m_bOneSkillStart)	// 3 초 동안 실행될 스킬의 시간을 계산한다.
	{
		m_dSkill_TimeAcc += TimeDelta;
		if (3 < m_dSkill_TimeAcc)
		{
			m_bOneSkillStart = false;

			CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
			m_bSkill_Clone = false;
			m_dSkill_TimeAcc = 0;
		}
	}

	// 스킬 한 번만 실행할 때
	if (!m_bSkill_Clone)																							// 처음 상태임! 객체를 생성하지 않았을 때
	{
		if (CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)		// 해당 스킬을
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_MARCELINE);			// 실행 시킨다.

		if (CSkill_Manager::PLAYERSKILL::COIN == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_COIN);

		if (CSkill_Manager::PLAYERSKILL::FOOD == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_FOOD);

		if (CSkill_Manager::PLAYERSKILL::FIONA == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)			// 변신 스킬의 경우 그 객체에서 모든 것을 처리한다.
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_FIONA);
	}
}

void CJake::Player_Follow(_double TimeDelta)
{
	// 현재 플레이어를 따라간다.

	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();	// Finn 좌표 받아옴

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos;											// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	if (2.2f > fDistanceX)
		m_pTransformCom->Chase(vPlayerPos, TimeDelta * 0.5, 1.5f, m_pNavigationCom);
	else
		m_pTransformCom->Chase(vPlayerPos, TimeDelta, 1.5f, m_pNavigationCom);

	// 수영 중 일 때는 Look 을 변경해서 따라간다. (플레이어가 수영 중 일 때는 Look 이 아래를 따라가서..)
	if (CObj_Manager::PLAYERINFO::SWIM == CObj_Manager::GetInstance()->Get_Current_Player().eState)		// 플레이어가 수영 중 인데
	{
		if (1 == m_pNavigationCom->Get_CellType())
			m_pTransformCom->LookAt(XMVectorSetY(CObj_Manager::GetInstance()->Get_Player_Transform(), -0.3f));	// 나도 수영 중 이라면 낮게 보고,
		else
			m_pTransformCom->LookAt(XMVectorSetY(CObj_Manager::GetInstance()->Get_Player_Transform(), 0.0f));	// 난 수영 중이 아닌데 플레이어가 수영 중 이라면 0.0f 를 바라보게 한다.
	}
	else
		m_pTransformCom->LookAt(XMVectorSetY(CObj_Manager::GetInstance()->Get_Player_Transform(), 0.0f));

	// 따라갈 때 애니메이션
	if (CObj_Manager::PLAYERINFO::STATE::RUN == CObj_Manager::GetInstance()->Get_Current_Player().eState ||
		CObj_Manager::PLAYERINFO::STATE::ROLL == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		if (1.5f < fDistanceX)
			m_tPlayerInfo.eState = m_tPlayerInfo.RUN;
	}
	else
	{
		if (1.5f > fDistanceX)
			m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
	}
}

void CJake::Check_Follow(_double TimeDelta)
{
	// 일정 시간 동안 내가 플레이어를 따라가지 않으면 플레이어 근처로 이동한다.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake 좌표 받아옴

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos; // 내 좌표가 객체를 바라보는 방향 벡터 (Finn <- Jake)

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	if (3.0f < fDistanceX)
	{
		m_dNotfollow_TimeAcc += TimeDelta;
		if (3.0 < m_dNotfollow_TimeAcc) // 따라오지 못 하는 시간이 5 초를 넘어간다면
		{
			_vector		vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float fLookX = XMVectorGetX(vMyLook);
			_float fLookZ = XMVectorGetZ(vMyLook);

			_float fAddX, fAddZ;
			if (0 < fLookX)		// +
				fAddX = 1.2f;
			else				// -
				fAddX = -1.2f;

			if (0 < fLookZ)
				fAddZ = 1.2f;
			else
				fAddZ = -1.2f;

			// 좌표 이동
			CNavigation * pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));

			_float4 f4Position;
			XMStoreFloat4(&f4Position, vPlayerPos);
			m_pNavigationCom->Set_CellIndex(pNavigationCom->Get_CellIndex());
			f4Position = { f4Position.x - fAddX, f4Position.y, f4Position.z - fAddZ, 1.f };
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4Position.x, f4Position.y, f4Position.z, f4Position.w), m_pNavigationCom);	// 플레이어 근처로 이동

			CEffect_Manager::GetInstance()->Effect_Smoke_Count(_float3(f4Position.x, f4Position.y + 0.7f, f4Position.z - 0.7f), _float3(0.968f, 0.729f, 0.160f), 50, { 0.3f, 0.7f });
			CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4Position.x, f4Position.y + 0.7f, f4Position.z - 0.7f));

			// 사운드
			pGameInstance->Play_Sound(TEXT("Hi_Jake.mp3"), 1.0f);

			m_dNotfollow_TimeAcc = 0;
		}
	}

	RELEASE_INSTANCE(CGameInstance);


	//// 일정시간 동안 Jake 가 근처에 있지 않다면 Jake 를 내 근처로 이동시킨다.
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), m_pTransformComTag, 0));

	//_vector vPlayerPos;
	//vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Finn 좌표 받아옴

	//_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	//_vector		vDir = vPlayerPos - vMyPos; // 내 좌표가 객체를 바라보는 방향 벡터 (Finn <- Jake)

	//_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	//if (5.f < fDistanceX)
	//{
	//	m_dNotfollow_TimeAcc += TimeDelta;
	//	if (3 < m_dNotfollow_TimeAcc) // 따라오지 못 하는 시간이 5 초를 넘어간다면
	//	{
	//		_vector		vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//		_float fLookX = XMVectorGetX(vMyLook);
	//		_float fLookZ = XMVectorGetZ(vMyLook);

	//		_float fAddX, fAddZ;
	//		if (0 < fLookX)		// +
	//			fAddX = 1.2f;
	//		else				// -
	//			fAddX = -1.2f;

	//		if (0 < fLookZ)
	//			fAddZ = 1.2f;
	//		else
	//			fAddZ = -1.2f;

	//		CNavigation * pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Jake"), TEXT("Prototype_Component_Navigation"), 0));

	//		_float4 f4MyPos;
	//		XMStoreFloat4(&f4MyPos, vMyPos);
	//		pJakeTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4MyPos.x - fAddX, f4MyPos.y, f4MyPos.z - fAddZ, 1.f), pNavigationCom);	// 내 옆으로 옮김
	//		
	//		m_dNotfollow_TimeAcc = 0;
	//	}
	//}

}

void CJake::Key_Input(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::CONTROL);
	}
	if (pGameInstance->Key_Up(DIK_LCONTROL))
	{
		CObj_Manager::GetInstance()->Set_Interaction(false);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
		CObj_Manager::GetInstance()->Set_Jake_Weapon(CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST);
	}

	if (true == CObj_Manager::GetInstance()->Get_Interaction() ||	// jake 의 경우 control 을 누를 수 있는데 그냥 그렇게 하장..
		m_tPlayerInfo.eState == m_tPlayerInfo.CONTROL ||
		m_tPlayerInfo.eState == m_tPlayerInfo.ROLL ||
		m_tPlayerInfo.eState == m_tPlayerInfo.HIT ||
		m_tPlayerInfo.eState == m_tPlayerInfo.STUN)
	{
		m_OnMove = false;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_OnMove)
	{
		if (CObj_Manager::PLAYERINFO::SWIM == CObj_Manager::GetInstance()->Get_Current_Player().eState)
			m_pTransformCom->Go_Straight(TimeDelta, 2.f, m_pNavigationCom);
		else
			m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);

		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::RUN);

		m_pTransformCom->PlayerMove(XMVectorSet(m_f4NewLook.x, m_f4NewLook.y, m_f4NewLook.z, m_f4NewLook.w), TimeDelta);
	}

#pragma region 이동
	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_bAttack_Sound = false;
		pGameInstance->Stop_Sound(1);
		
		m_OnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, 1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_bAttack_Sound = false;
		pGameInstance->Stop_Sound(1);
		
		m_OnMove = true;
		m_f4NewLook = { 1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_bAttack_Sound = false;
		pGameInstance->Stop_Sound(1);
		
		m_OnMove = true;
		m_f4NewLook = { 0.0f, 0.0f, -1.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_RIGHT))
			m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_LEFT))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}
	if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_bAttack_Sound = false;
		pGameInstance->Stop_Sound(1);
		
		m_OnMove = true;
		m_f4NewLook = { -1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}

	if (pGameInstance->Key_Up(DIK_UP) || pGameInstance->Key_Up(DIK_RIGHT) || pGameInstance->Key_Up(DIK_DOWN) || pGameInstance->Key_Up(DIK_LEFT))
	{
		m_OnMove = false;
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
#pragma endregion

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_bSkill &&
			CSkill_Manager::PLAYERSKILL::PAINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		{
			m_bSkill_Clone = false;
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::S_PAINT);
		}
		else
		{
			if (false == m_bAttack_Sound)
			{
				m_bAttack_Sound = true;
				pGameInstance->Play_Sound(TEXT("Jake_Attack.mp3"), 0.7f, false, 1);
			}
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);
		}
	}

	if (pGameInstance->Key_Down(DIK_LSHIFT))
	{
		pGameInstance->Stop_Sound(1);
		pGameInstance->Play_Sound(TEXT("roll.ogg"), 0.7f);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ROLL);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CJake::Space_Attack_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
	{
		m_bAttack_Sound = false;
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}
}

void CJake::Attack_Paint_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;

	if (18 == m_pModelCom->Get_AnimIndex() && 0 <= m_pModelCom->Get_Keyframes())
		return;

	// 1) 총 스킬이 재생되어야 하는 시간을 체크하는 Tick
	// 2) 스페이스바를 누르면 그 때 마다 객체가 생성되어야 하는 Tick

	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 스킬 생성
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CS_PaintWork::PAINTWORKINFO		tPaintWorkInfo;

		_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(-15.f));
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook = XMVector4Transform(vLook, RotationMatrix);		// 회전행렬의 look 을 가져온다.
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);			// 넘긴다.

		tPaintWorkInfo.fAttack = CObj_Manager::GetInstance()->Get_Player_Attack();	// 공격력은 일부러 한 번만 넘긴다.
		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.BLUE;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_0"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);

		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.MAGENTA;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_1"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(15.f));
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook = XMVector4Transform(vLook, RotationMatrix);
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);

		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.YELLOW;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_2"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CJake::Skill_Marceline_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Marceline_1"), TEXT("Prototype_GameObject_S_Marceline"),
			&_float3(f4MyPos.x + 1.0f, f4MyPos.y, f4MyPos.z + 1.0f))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CJake::Skill_Coin_Tick(_double TimeDelta)
{
	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);
		CItemManager::GetInstance()->RandomCoin_Clone(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), 5, 5, 12); 	// 동전 생성
	}
}

void CJake::Skill_Food_Tick(_double TimeDelta)
{
	if (false == m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 내 Look 을 기준으로 거리 1만큼을 더 한 위치에 생성 시키고 싶다.
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		XMVector3Normalize(vLook * 1.0f);
		_float4 f4Look;
		XMStoreFloat4(&f4Look, vLook);
		CItemManager::GetInstance()->Food_Clone(_float3(f4MyPos.x + f4Look.x, f4MyPos.y + f4Look.y, f4MyPos.z + f4Look.z));
	}
}

HRESULT CJake::Skill_Fiona_Tick(_double TimeDelta)
{
	// 모델 생성
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(),								// STATIC 레벨에
		TEXT("Layer_S_Fiona"), TEXT("Prototype_GameObject_S_Fiona"), &_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z))))	// 스킬 객체를 생성한다.
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(true);

	return S_OK;
}

void CJake::Control_Tick(_double TimeDelta)
{
	m_pTransformCom->Go_Straight(0, m_pNavigationCom);

	CObj_Manager::GetInstance()->Set_Jake_Weapon(CObj_Manager::PLAYERINFO::JAKEWEAPON::SHIELD);
}

void CJake::Roolling_Tick(_double TimeDelta)
{
	if (m_tPlayerInfo.eState == m_tPlayerInfo.STUN)
		return;

	m_OnMove = false;	// 이동 누르고 shift 누르면 계속 직진해서 flase 로 바꿈

	if (!m_pModelCom->Get_Finished())
		m_pTransformCom->Go_Straight(TimeDelta, 4.f, m_pNavigationCom);
	else
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
}

void CJake::Hit_Tick(_double TimeDelta)
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
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CJake::KnockBack_Hit_Tick(_double TimeDelta)
{
	m_OnMove = false;

	m_dKnockBack_Hit_TimeAcc += TimeDelta;
	if (0.35 < m_dKnockBack_Hit_TimeAcc)
		m_pTransformCom->Go_Backward(0, m_pNavigationCom);
	else
		m_pTransformCom->Go_Backward(TimeDelta, m_pNavigationCom);

	if (m_pModelCom->Get_Finished())
	{
		m_dKnockBack_Hit_TimeAcc = 0; 
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CJake::Stun_Tick()
{
	_vector vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	if (!m_bStru)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("S_StunChick_1"), TEXT("Prototype_GameObject_S_StunChick"), &_float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		m_bStru = true;
	}

	if (m_pModelCom->Get_Finished())
		++m_iStun_Count;

	if (2 <= m_iStun_Count)		// 애니메이션 두 번 재생 후 끝
	{
		m_bStru = false;
		m_iStun_Count = 0;
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CJake::Swim_Tick(_double TimeDelta)
{
	CObj_Manager::GetInstance()->Set_Interaction(false);	// 보스전 할 때 보스 스킬 피하기 위해서 들어간다.

	// 수영 이펙트
	m_dEffect_Swim_TimeAcc += TimeDelta;
	if (0.5 < m_dEffect_Swim_TimeAcc)
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CEffect_Manager::GetInstance()->Effect_Swim_Create(_float3(f4MyPos.x, 0.5f, f4MyPos.z));
		m_dEffect_Swim_TimeAcc = 0;
	}

	// 수영
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::SWIM);

	if (!m_bDiving)
		m_pModelCom->Set_AnimIndex(43, false);	// DIVING

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	static _bool bSound;
	if (false == bSound)
	{
		bSound = true;
		pGameInstance->Play_Sound(TEXT("v_Jake_OutOfWater.ogg"), 1.0f);
	}

	if (43 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	{
		m_bDiving = true;
	}

	if (m_bDiving)
	{
		static _bool bSounding;
		if (false == bSounding)
		{
			bSounding = true;
			pGameInstance->Play_Sound(TEXT("sfx_character_drowning.ogg"), 1.0f, true, 3);
		}

		m_pModelCom->Set_AnimIndex(57);			// SWIM

		// CellType 이 1 이라면 내라가다가.
		m_pTransformCom->Go_SwinDown(TimeDelta * 2.0f, -0.6f);	// -0.6 변경하면 Player Follow 에서도 변경

		// CellType 이 0 이되면 올라간다.
		if (0 == m_pNavigationCom->Get_CellType())
		{
			m_pModelCom->Set_AnimIndex(53);		// RUN
			if (m_pTransformCom->Go_SwinUp(TimeDelta, 5.f))	// 0 까지 올라왔다면
			{
				bSound = false;
				bSounding = false;
				pGameInstance->Stop_Sound(3);
				pGameInstance->Play_Sound(TEXT("sfx_character_splah.ogg"), 1.0f);
				
				m_bDiving = false;
				m_bIsSwim = false;
				CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::IDLE);
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CJake::Change_Tick()
{
	if (CObj_Manager::PLAYERINFO::STATE::CHANGE != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return;

	m_OnMove = false;
	m_bIsSwim = false;
	m_tPlayerInfo.eState = m_tPlayerInfo.CHANGE;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
}

void CJake::Cheering_Tick()
{
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		return;

	if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return;

	m_tPlayerInfo.eState = m_tPlayerInfo.CHEERING;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
}

HRESULT CJake::Magic_Tick(_double TimeDelta)
{
	if (true == m_bChange)
		return S_OK;

	m_bChange = true;

	// 모델 생성
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,							
		TEXT("Layer_S_Change_Magic_JAKE"), TEXT("Prototype_GameObject_S_Change_Magic"), &_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z))))	// 스킬 객체를 생성한다.
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CJake::JakeSon_Tick(const _double & TimeDelta)
{
	if (true == m_bChange)
		return S_OK;

	m_bChange = true;

	// 모델 생성
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS,
		TEXT("Layer_S_Change_JakeSonTransform"), TEXT("Prototype_GameObject_S_JakeSonTransform"), &_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z))))	// 스킬 객체를 생성한다.
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

void CJake::JakeSon_Transform_Change()
{
	if (LEVEL_SKELETON_BOSS == CObj_Manager::GetInstance()->Get_Current_Level())	// 해당 레벨에서
	{	
		if (CSkill_Manager::PLAYERSKILL::JAKESON == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)	// 스킬을 사용하면
			m_tPlayerInfo.eState = CObj_Manager::PLAYERINFO::JAKESON;
	}
}

void CJake::BossCage(const _double TimeDelta)
{
	if (false == CObj_Manager::GetInstance()->Get_BossCage())
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4Pos, vPos);

	if (1.0f > f4Pos.x || 9.5f < f4Pos.x || 14.0f > f4Pos.z || 18.0f < f4Pos.z)
	{
		m_dBossCage_TimeAcc += TimeDelta;
		if (0.1 < m_dBossCage_TimeAcc)
			m_pTransformCom->Go_Backward(TimeDelta, m_pNavigationCom);
	}
	else
		m_dBossCage_TimeAcc = 0;
}

void CJake::Current_HP(const _double & TimeDelta)
{
	m_bShader_Hit = false;
	static _bool	bRevive;

	if (0 >= CObj_Manager::GetInstance()->Get_Current_Player().fHP)
	{
		m_pModelCom->Set_AnimIndex(45, false);

		if (1 == m_fAlpha)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("v_Jake_Dying.ogg"), 1.0f);
			pGameInstance->Play_Sound(TEXT("v_Finn_GameOver_Jake.ogg"), 1.0f);
			RELEASE_INSTANCE(CGameInstance);

		}

		if (0 < m_fAlpha)
			m_fAlpha -= _float(TimeDelta) * 1.5f;

		if (0 > m_fAlpha)
		{
			CObj_Manager::GetInstance()->Set_Heart(-1);
			CObj_Manager::GetInstance()->Set_Player_PlusHP(CObj_Manager::GetInstance()->Get_Current_Player().fHPMax);
			bRevive = true;
		}
	}
	else
	{
		if (false == bRevive)
			return;

		m_pModelCom->Set_AnimIndex(44, false);

		if (1 > m_fAlpha)
			m_fAlpha += _float(TimeDelta) * 1.5f;

		if (10 <= m_pModelCom->Get_Keyframes())	// TODO : 나중에 릴리즈로 돌리
		{
			m_fAlpha = 1;
			bRevive = false;
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
			CObj_Manager::GetInstance()->Set_Interaction(false);	// 공격 받다가 죽기 때문에 false 로 변경해 준다.
		}
	}
}

void CJake::Anim_Change(_double TimeDelta)
{
	if (m_tPlayerInfo.ePreState != m_tPlayerInfo.eState)
	{
		switch (m_tPlayerInfo.eState)
		{
		case CObj_Manager::PLAYERINFO::STATE::IDLE:
			m_pModelCom->Set_AnimIndex(42);
			break;

		case CObj_Manager::PLAYERINFO::STATE::RUN:
			m_pModelCom->Set_AnimIndex(53);
			break;

		case CObj_Manager::PLAYERINFO::STATE::ROLL:
			m_pModelCom->Set_AnimIndex(52, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::ATTACK:
			m_pModelCom->Set_AnimIndex(4, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::S_PAINT:
			m_pModelCom->Set_AnimIndex(18, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::S_MARCELINE:
			m_pModelCom->Set_AnimIndex(27, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::HIT:
			m_pModelCom->Set_AnimIndex(38, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::KNOCKBACKHIT:
			m_pModelCom->Set_AnimIndex(50, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::STUN:
			m_pModelCom->Set_AnimIndex(56, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::CONTROL:
			m_pModelCom->Set_AnimIndex(6);
			break;

		case CObj_Manager::PLAYERINFO::STATE::CHANGE:
			m_pModelCom->Set_AnimIndex(33, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::CHEERING:
			m_pModelCom->Set_AnimIndex(9, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::LEVEL_UP:
			m_pModelCom->Set_AnimIndex(47, true, false);
			break;
		}

		if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)	// 내가 현재 플레이어 일 때만 상태를 변경한다.
			CObj_Manager::GetInstance()->Set_Current_Player_State(m_tPlayerInfo.eState);
		m_tPlayerInfo.ePreState = m_tPlayerInfo.eState;
	}
}

HRESULT CJake::Talk(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_C))
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CPlayer_Talk::PLAYERTALKINFO	 tPlayerInfo;
		tPlayerInfo.eTextureType = CPlayer_Talk::PLAYERTALKINFO::TYPE::JAKE;
		tPlayerInfo.pTarget_TransformCom = m_pTransformCom;

		if (FAILED(pGameInstance->Clone_GameObject(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Layer_PlayerTalk"), TEXT("Prototype_GameObject_Player_Talk"), &tPlayerInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}


	m_dTalk_TimeAcc += TimeDelta;
	if (20 < m_dTalk_TimeAcc)
		m_dTalk_TimeAcc = 0;

	if (0 == m_dTalk_TimeAcc)
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CPlayer_Talk::PLAYERTALKINFO	 tPlayerInfo;
		tPlayerInfo.eTextureType = CPlayer_Talk::PLAYERTALKINFO::TYPE::JAKE;
		tPlayerInfo.pTarget_TransformCom = m_pTransformCom;

		if (FAILED(pGameInstance->Clone_GameObject(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Layer_PlayerTalk"), TEXT("Prototype_GameObject_Player_Talk"), &tPlayerInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CJake::Sound_Tick()
{
	//if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)
	//{
	//	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//	pGameInstance->Stop_Sound(1);
	//	RELEASE_INSTANCE(CGameInstance);
	//}
}

CJake * CJake::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CJake*		pInstance = new CJake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CJake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJake::Clone(void * pArg)
{
	CJake*		pInstance = new CJake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CJake");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJake::Free()
{
	__super::Free();

	for (auto& pPart : m_PlayerParts)
		Safe_Release(pPart);
	m_PlayerParts.clear();

	Safe_Release(m_pShaderXRayCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
