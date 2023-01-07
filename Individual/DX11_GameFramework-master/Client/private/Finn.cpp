#include "stdafx.h"
#include "..\public\Finn.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Finn_Weapon.h"

#include "ItemManager.h"
#include "Skill_Manager.h"
#include "S_PaintWork.h"
#include "S_Fiona.h"

CFinn::CFinn(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CFinn::CFinn(const CFinn & rhs)
	: CGameObject(rhs)
{

}

HRESULT CFinn::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFinn::Initialize(void * pArg)
{
	m_wsTag = L"Finn";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(39);

	m_tPlayerInfo.ePlayer = m_tPlayerInfo.FINN;
	m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;

	return S_OK;
}

void CFinn::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//Shader_Time(TimeDelta); // Shader Hit Time

	// 내가 현재 플레이어가 아니라면 무기를 호출하지 않는다.
	if (CObj_Manager::PLAYERINFO::STATE::S_FIONA != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		Parts_Tick(TimeDelta);

	Current_Player(TimeDelta);
	Player_Tick(TimeDelta);

	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);
		m_pColliderCom[COLLTYPE_AABB]->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

void CFinn::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 내가 현재 플레이어가 아니라면 무기를 호출하지 않는다.
	if (CObj_Manager::PLAYERINFO::STATE::S_FIONA != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		Parts_LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFinn::Render()
{
	//// 플레이어 스킬 상태일때 Player 의 Render 를 잠시 꺼둔다.
	//if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer && 
	//	CObj_Manager::PLAYERINFO::STATE::MAGIC == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	//	return E_FAIL;

	// 내가 현재 플레이어가 아니더라도 해당 스킬을 사용하면 렌더를 끈다.
	if (CObj_Manager::PLAYERINFO::STATE::S_FIONA == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return E_FAIL;

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
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		for (_uint i = 0; i < COLLTYPE_END; ++i)
		{
			if (nullptr != m_pColliderCom[i])
				m_pColliderCom[i]->Render();
		}

		m_pNavigationCom->Render();
	}

	return S_OK;
}

void CFinn::On_Collision(CGameObject * pOther)
{
	CSkill_Manager::GetInstance()->Page_PickUp(pOther);
}

HRESULT CFinn::SetUp_Components()
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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_Finn"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.4f, 1.2f, 0.4f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom[COLLTYPE_AABB], &ColliderDesc)))
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

HRESULT CFinn::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Key_Pressing(DIK_F))
	{
		m_bHit = true;
		if (FAILED(m_pShaderCom->Set_RawValue("g_bHit", &m_bHit, sizeof _bool)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CFinn::Shader_Time(_double TimeDelta)
{
	if (!m_bHit)
		return;

	//if (m_bHit)
	//{
	//	m_bHit_TimeAcc += TimeDelta;
	//	if (0.5 < m_bHit_TimeAcc)
	//	{
	//		m_bHit = false;
	//		m_bHit_TimeAcc = 0;
	//	}
	//}
}

HRESULT CFinn::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CFinn_Weapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CFinn_Weapon::WEAPONDESC));

	WeaponDesc.eSwordType = CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_ROOT;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Sword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Finn_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	WeaponDesc.eSwordType = CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_DOLDEN;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Sword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Finn_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	WeaponDesc.eSwordType = CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_FAMILY;
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Sword");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Finn_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CFinn::Parts_Tick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_ROOT == CObj_Manager::GetInstance()->Get_Current_Player().ePlayerWeapon)
		m_PlayerParts[0]->Tick(TimeDelta);
	else if (CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_DOLDEN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayerWeapon)
		m_PlayerParts[1]->Tick(TimeDelta);
	else if (CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_FAMILY == CObj_Manager::GetInstance()->Get_Current_Player().ePlayerWeapon)
		m_PlayerParts[2]->Tick(TimeDelta);
}

void CFinn::Parts_LateTick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_ROOT == CObj_Manager::GetInstance()->Get_Current_Player().ePlayerWeapon)
		m_PlayerParts[0]->Late_Tick(TimeDelta);
	else if (CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_DOLDEN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayerWeapon)
		m_PlayerParts[1]->Late_Tick(TimeDelta);
	else if (CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_FAMILY == CObj_Manager::GetInstance()->Get_Current_Player().ePlayerWeapon)
		m_PlayerParts[2]->Late_Tick(TimeDelta);
}

void CFinn::Player_Tick(_double TimeDelta)
{	
	// 내가 플레이어가 아닐 때에도 해야하는 행동
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

	// 내가 플레이어 일 때 만 할 행동	
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

	case CObj_Manager::PLAYERINFO::S_FIONA:	// 14
		Skill_Fiona_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::ROLL:
		Roolling_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::HIT:
		Hit_Tick(TimeDelta);
		break;

	case CObj_Manager::PLAYERINFO::STUN:
		Stun_Tick();
		break;
	}

	Anim_Change(TimeDelta);
}

void CFinn::Current_Player(_double TimeDelta)
{
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)					// Player 나라면
	{
		CObj_Manager::GetInstance()->Tick_Player_Transform();		// 현재 플레이어의 좌표를 Tick
		Player_Skill_Tick(TimeDelta);

		// 플레이어의 스킬 때 키 입력을 받지 않는다.
		if (CObj_Manager::PLAYERINFO::STATE::MAGIC == CObj_Manager::GetInstance()->Get_Current_Player().eState ||
			CObj_Manager::PLAYERINFO::STATE::S_FIONA == CObj_Manager::GetInstance()->Get_Current_Player().eState)
			m_OnMove = false; 
		else
			Key_Input(TimeDelta);
	}
	else
	{
		Player_Follow(TimeDelta);									// Player 가 내가 아니라면 따라간다.								
		Check_Follow(TimeDelta);									// Player 근처에 내가 있는지 확인한다. 수정 : 내 주변 셀을 입력한다.
	}
}

void CFinn::Player_Skill_Tick(_double TimeDelta)
{
	// 전체적으로 스킬을 on 한다.
	if (CSkill_Manager::PLAYERSKILL::SKILL_END != CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		m_bSkill = true;

	if (m_bSkill)	// COIN 한번만 생성되고, 추가적인 제어 때문에 직접 함수 안에서 처리한다.
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

	// 스킬 한 번만 실행할 때
	if (!m_bSkill_Clone)																							// 처음 상태임! 객체를 생성하지 않았을 때
	{
		if (CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)		// 해당 스킬을
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_MARCELINE);			// 실행 시킨다.
	
		if (CSkill_Manager::PLAYERSKILL::COIN == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_COIN);

		if (CSkill_Manager::PLAYERSKILL::FIONA == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_FIONA);
	}
}

void CFinn::Player_Follow(_double TimeDelta)
{
	// 현재 플레이어를 따라간다.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Jake 에게로
	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();			// Jake 좌표 받아옴

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos;												// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));						// X 값을 뽑아와 거리 확인

	if (2.2f > fDistanceX)
		m_pTransformCom->Chase(vPlayerPos, TimeDelta * 0.5, 1.5f, m_pNavigationCom);
	else
		m_pTransformCom->Chase(vPlayerPos, TimeDelta, 1.5f, m_pNavigationCom);

	// 수영 중 일 때는 Look 을 변경해서 따라간다. (플레이어가 수영 중 일 때는 Look 이 아래를 따라가서..)
	if (CObj_Manager::PLAYERINFO::SWIM == CObj_Manager::GetInstance()->Get_Current_Player().eState)		// 플레이어가 수영 중 인데
	{
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);
		if (1 == m_pNavigationCom->Get_CellType())
			m_pTransformCom->LookAt(XMVectorSet(f4PlayerPos.x, -0.8f, f4PlayerPos.z, f4PlayerPos.w));	// 나도 수영 중 이라면 낮게 보고,
		else
			m_pTransformCom->LookAt(XMVectorSet(f4PlayerPos.x, 0.f, f4PlayerPos.z, f4PlayerPos.w));		// 난 수영 중이 아니라면 0 을 본다.
	}
	else
		m_pTransformCom->LookAt(vPlayerPos);

	// 따라갈 때 애니메이션
	if (CObj_Manager::PLAYERINFO::STATE::RUN == CObj_Manager::GetInstance()->Get_Current_Player().eState ||
		CObj_Manager::PLAYERINFO::STATE::ROLL == CObj_Manager::GetInstance()->Get_Current_Player().eState ||
		CObj_Manager::PLAYERINFO::STATE::MAGIC == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		if (1.5f < fDistanceX)
			m_tPlayerInfo.eState = m_tPlayerInfo.RUN;
	}
	else
	{
		if (1.5f > fDistanceX)
			m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Check_Follow(_double TimeDelta)
{
	// 일정 시간 동안 내가 플레이어를 따라가지 않으면 플레이어 근처로 이동한다.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake 좌표 받아옴

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos; // 내 좌표가 객체를 바라보는 방향 벡터 (Finn <- Jake)

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	if (3.f < fDistanceX)
	{
		m_dNotfollow_TimeAcc += TimeDelta;
		if (3 < m_dNotfollow_TimeAcc) // 따라오지 못 하는 시간이 5 초를 넘어간다면
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

			CNavigation * pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));

			_float4 f4MyPos;
			XMStoreFloat4(&f4MyPos, vPlayerPos);
			m_pNavigationCom->Set_CellIndex(pNavigationCom->Get_CellIndex());
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4MyPos.x - fAddX, f4MyPos.y, f4MyPos.z - fAddZ, 1.f), m_pNavigationCom);	// 플레이어 근처로 이동

			m_dNotfollow_TimeAcc = 0;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Key_Input(_double TimeDelta)
{
	if (true == CObj_Manager::GetInstance()->Get_Interaction() ||
		m_tPlayerInfo.eState == m_tPlayerInfo.ROLL	||
		m_tPlayerInfo.eState == m_tPlayerInfo.HIT	||
		m_tPlayerInfo.eState == m_tPlayerInfo.STUN	||
		m_tPlayerInfo.eState == m_tPlayerInfo.TREEWITCH_0 || m_tPlayerInfo.eState == m_tPlayerInfo.TREEWITCH_1 || m_tPlayerInfo.eState == m_tPlayerInfo.TREEWITCH_2)
	{
		m_OnMove = false;
		return;
	}

	if (m_OnMove)
	{
		if (CObj_Manager::PLAYERINFO::SWIM == CObj_Manager::GetInstance()->Get_Current_Player().eState)
			m_pTransformCom->Go_Straight(TimeDelta, 2.f, m_pNavigationCom);
		else
			m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);

		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::RUN);
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);
	}

	if (pGameInstance->Key_Down(DIK_LSHIFT))
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ROLL);

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Space_Attack_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
}

void CFinn::Attack_Paint_Tick(_double TimeDelta)
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

		tPaintWorkInfo.iAttack = CObj_Manager::GetInstance()->Get_Player_Attack();	// 공격력은 일부러 한 번만 넘긴다.
		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.BLUE;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_0"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
			return;

		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);

		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.MAGENTA;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_1"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
			return;

		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(15.f));
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook = XMVector4Transform(vLook, RotationMatrix);
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);

		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.YELLOW;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.7f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_2"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CFinn::Skill_Marceline_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Marceline_0"), TEXT("Prototype_GameObject_S_Marceline"),
			&_float3(f4MyPos.x + 1.0f, f4MyPos.y, f4MyPos.z + 1.0f))))
			return;
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CFinn::Skill_Coin_Tick(_double TimeDelta)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CItemManager::GetInstance()->RandomCoin_Clone(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), 3, 3, 6); 	// 동전 생성

	CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
	m_bSkill = false;
}

HRESULT CFinn::Skill_Fiona_Tick(_double TimeDelta)
{
	if (m_bIsSwim)		// 예외처리 수영 중일 때는 변하지 말기
		return S_OK;

	// m_bSkill_Clone -> ture 라면? KeyInput(), Render() 를 호출하지 않는다.
	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		// 모델 생성
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(),								// STATIC 레벨에
			TEXT("Layer_S_Fiona"), TEXT("Prototype_GameObject_S_Fiona"), &_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z))))	// 스킬 객체를 생성한다.
			return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
	}

	// 모델을 따라간다. (우리 눈에는 보이지 않는다.)
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform * pChangeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(),	// STATIC 레벨에
		TEXT("Layer_S_Fiona"), TEXT("Com_Transform"), 0));																// 스킬 객체의 트랜스폼을 가져온다.

	if (nullptr != pChangeTransformCom)	// 예외처리) 만약 객체가 생성되지 않았다면, 따라가지 않는다.
	{
		_vector vChangePos = pChangeTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vChangePos);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CFinn::Roolling_Tick(_double TimeDelta)
{
	if (m_tPlayerInfo.eState == m_tPlayerInfo.STUN)
		return;

	m_OnMove = false;	// 이동 누르고 shift 누르면 계속 직진해서 flase 로 바꿈

	if (!m_pModelCom->Get_Finished())
		m_pTransformCom->Go_Straight(TimeDelta, 4.f, m_pNavigationCom);
	else
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
}

void CFinn::Hit_Tick(_double TimeDelta)
{
	m_OnMove = false;

	m_dHit_TimeAcc += TimeDelta;
	if (0.22 < m_dHit_TimeAcc)
		m_pTransformCom->Go_Backward(0, m_pNavigationCom);
	else
		m_pTransformCom->Go_Backward(TimeDelta, m_pNavigationCom);

	if (m_pModelCom->Get_Finished())
	{
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		m_dHit_TimeAcc = 0;
	}
}

void CFinn::Stun_Tick()
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	if (!m_bStun)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("S_StunChick_0"), TEXT("Prototype_GameObject_S_StunChick"), &_float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z))))
			return;
		RELEASE_INSTANCE(CGameInstance);

		m_bStun = true;
	}

	if (m_pModelCom->Get_Finished())
		++m_iStun_Count;

	if (2 <= m_iStun_Count)		// 애니메이션 두 번 재생 후 끝
	{
		m_bStun = false;
		m_iStun_Count = 0;
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
	}
}

void CFinn::Swim_Tick(_double TimeDelta)
{
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::SWIM);

	if(!m_bDiving)
		m_pModelCom->Set_AnimIndex(40, false);

	if (40 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_bDiving = true;
		
	if (m_bDiving)
	{
		m_pModelCom->Set_AnimIndex(52);

		// CellType 이 1 이라면 내려가다가
		m_pTransformCom->Go_SwinDown(TimeDelta, 1.2f, -0.8f);	// -0.6 변경하면 Player Flolow() 에서도 변경

		// CellType 이 0 이되면 올라간다.
		if (0 == m_pNavigationCom->Get_CellType())
		{
			m_pModelCom->Set_AnimIndex(49);	
			if (m_pTransformCom->Go_SwinUp(TimeDelta, 5.f))	// 0 까지 올라왔다면
			{
				m_bDiving = false;
				m_bIsSwim = false;
			}
		}
	}
}

void CFinn::Change_Tick()
{
	if (CObj_Manager::PLAYERINFO::STATE::CHANGE != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return;

	m_OnMove = false;
	m_bIsSwim = false;
	m_tPlayerInfo.eState = m_tPlayerInfo.CHANGE;

	if (m_pModelCom->Get_Finished())
		CObj_Manager::GetInstance()->Set_Current_Player_State(m_tPlayerInfo.IDLE);
}

void CFinn::Cheering_Tick()
{
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		return;

	if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return;

	m_tPlayerInfo.eState = m_tPlayerInfo.CHEERING;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
}

void CFinn::Anim_Change(_double TimeDelta)
{
	if (m_tPlayerInfo.ePreState != m_tPlayerInfo.eState)
	{
		switch (m_tPlayerInfo.eState)
		{
		case CObj_Manager::PLAYERINFO::STATE::IDLE:
			m_pModelCom->Set_AnimIndex(39, true, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::RUN:
			m_pModelCom->Set_AnimIndex(49, true, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::ROLL:
			m_pModelCom->Set_AnimIndex(48, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::ATTACK:
			m_pModelCom->Set_AnimIndex(5, false, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::S_PAINT:
			m_pModelCom->Set_AnimIndex(18, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::S_MARCELINE:
			m_pModelCom->Set_AnimIndex(25);
			break;

		//case CObj_Manager::PLAYERINFO::STATE::S_COIN:
		//	m_pModelCom->Set_AnimIndex(23, false);
		//	break;

		case CObj_Manager::PLAYERINFO::STATE::TREEWITCH_0:
			m_pModelCom->Set_AnimIndex(55, false, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::TREEWITCH_1:
			m_pModelCom->Set_AnimIndex(54, false, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::TREEWITCH_2:
			m_pModelCom->Set_AnimIndex(53, false, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::HIT:
			m_pModelCom->Set_AnimIndex(35, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::STUN:
			m_pModelCom->Set_AnimIndex(51, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::CHANGE:
			m_pModelCom->Set_AnimIndex(31, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::CHEERING:
			m_pModelCom->Set_AnimIndex(14, false);
			break;
		}

		if(m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
			CObj_Manager::GetInstance()->Set_Current_Player_State(m_tPlayerInfo.eState);
		m_tPlayerInfo.ePreState = m_tPlayerInfo.eState;
	}
}

CFinn * CFinn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFinn*		pInstance = new CFinn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFinn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFinn::Clone(void * pArg)
{
	CFinn*		pInstance = new CFinn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFinn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFinn::Free()
{
	__super::Free();

	for (auto& pPart : m_PlayerParts)
		Safe_Release(pPart);
	m_PlayerParts.clear();

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
