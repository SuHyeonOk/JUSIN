#include "stdafx.h"
#include "..\public\Jake.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Jake_Weapon.h"

#include "O_TextureObject.h"
#include "Skill_Manager.h"
#include "S_Change_Magic.h"
#include "S_PaintWork.h"

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

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(18);

	m_tPlayerInfo.ePlayer	= m_tPlayerInfo.JAKE;
	m_tPlayerInfo.eState	= m_tPlayerInfo.IDLE;

	return S_OK;
}

void CJake::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Sword_Tick(TimeDelta);

	Current_Player(TimeDelta);
	Player_Tick(TimeDelta);

	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

void CJake::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Sword_LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CJake::Render()
{
	// 플레이어 스킬 상태일때 Player 의 Render 를 잠시 꺼둔다.
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer &&
		CObj_Manager::PLAYERINFO::STATE::MAGIC == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return E_FAIL;

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
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();

		m_pNavigationCom->Render();
	}

	return S_OK;
}

void CJake::On_Collision(CGameObject * pOther)
{
	CSkill_Manager::GetInstance()->Page_PickUp(pOther);
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_Jake"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.35f, 0.7f, 0.35f);
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


	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	
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

	WeaponDesc.eWeaponType = CObj_Manager::PLAYERINFO::JAKEWEAPON::SHLDE;
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

void CJake::Sword_Tick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon ||
		CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
	{
		m_PlayerParts[0]->Tick(TimeDelta);
		m_PlayerParts[1]->Tick(TimeDelta);
	}
	else if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHLDE == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
		m_PlayerParts[2]->Tick(TimeDelta);
}

void CJake::Sword_LateTick(const _double & TimeDelta)
{
	if (CObj_Manager::PLAYERINFO::JAKEWEAPON::LFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon ||
		CObj_Manager::PLAYERINFO::JAKEWEAPON::RFIST == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
	{
		m_PlayerParts[0]->Late_Tick(TimeDelta);
		m_PlayerParts[1]->Late_Tick(TimeDelta);
	}
	else if (CObj_Manager::PLAYERINFO::JAKEWEAPON::SHLDE == CObj_Manager::GetInstance()->Get_Current_Player().eJakeWeapon)
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

	case CObj_Manager::PLAYERINFO::CONTROL:
		Control_Tick(TimeDelta);
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

	case CObj_Manager::PLAYERINFO::MAGIC:
		Magic_Tick(TimeDelta);
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

		// 플레이어의 스킬 때 키 입력을 받지 않는다.
		if (CObj_Manager::PLAYERINFO::STATE::MAGIC != CObj_Manager::GetInstance()->Get_Current_Player().eState)
			Key_Input(TimeDelta);
		else
			m_OnMove = false;
	}
	else
	{
		Player_Follow(TimeDelta);
		Check_Follow(TimeDelta);
	}
}

void CJake::Player_Skill_Tick(_double TimeDelta)
{
	// 전체적으로 스킬을 on 한다.
	if (CSkill_Manager::PLAYERSKILL::SKILL_END != CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
	{
		m_bSkill = true;
	}

	if (m_bSkill)
	{
		m_dSkill_TimeAcc += TimeDelta;
		if (20 < m_dSkill_TimeAcc)
		{
			// 모든 스킬을 false 로 변경한다. (예외적으로 키 입력을 하는 경우는 추가 처리)
			m_bSkill_Clone = false;

			CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
			m_bSkill = false;
			m_dSkill_TimeAcc = 0;
		}
	}

	// 스킬 한 번만 실행할 때
	if (!m_bSkill_Clone && CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_MARCELINE);
}

void CJake::Player_Follow(_double TimeDelta)
{
	// 현재 플레이어를 따라간다.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Finn 에게로
	//CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), m_pTransformComTag, 0));

	//_vector vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// Finn 좌표 받아옴
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
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);
		if (1 == m_pNavigationCom->Get_CellType())
			m_pTransformCom->LookAt(XMVectorSet(f4PlayerPos.x, -0.3f, f4PlayerPos.z, f4PlayerPos.w));	// 나도 수영 중 이라면 낮게 보고,
		else
			m_pTransformCom->LookAt(XMVectorSet(f4PlayerPos.x, 0.f, f4PlayerPos.z, f4PlayerPos.w));		// 난 수영 중이 아니라면 0 을 본다.
	}
	else
		m_pTransformCom->LookAt(vPlayerPos);

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

	RELEASE_INSTANCE(CGameInstance);
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

			// 좌표 이동
			CNavigation * pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));

			_float4 f4MyPos;
			XMStoreFloat4(&f4MyPos, vPlayerPos);
			m_pNavigationCom->Set_CellIndex(pNavigationCom->Get_CellIndex());
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4MyPos.x - fAddX, f4MyPos.y, f4MyPos.z - fAddZ, 1.f), m_pNavigationCom);	// 플레이어 근처로 이동

			// 오브젝트 포탈 생성
			CO_TextureObject::TEXTUREOBJECT tTextureObject;
			tTextureObject.eTextureType = tTextureObject.MOVE_PORTAL;
			tTextureObject.f3Pos = _float3(f4MyPos.x - fAddX, f4MyPos.y, (f4MyPos.z - fAddZ) - 0.5f);
			if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Portal_Jake"), TEXT("Prototype_GameObject_O_TextureObject"), &tTextureObject)))
				return;

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
		if (m_bSkill)
		{
			if (CSkill_Manager::PLAYERSKILL::PAINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			{
				m_bSkill_Clone = false;
				CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::S_PAINT);
			}
		}
		else
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);
	}

	if (pGameInstance->Key_Down(DIK_LSHIFT))
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ROLL);

	RELEASE_INSTANCE(CGameInstance);
}

void CJake::Space_Attack_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
}

void CJake::Attack_Paint_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;

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
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_0"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
			return;

		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);

		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.MAGENTA;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_1"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
			return;

		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(15.f));
		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook = XMVector4Transform(vLook, RotationMatrix);
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);

		tPaintWorkInfo.ePaintWork = tPaintWorkInfo.YELLOW;
		tPaintWorkInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y + 0.5f, f4MyPos.z);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Paint_2"), TEXT("Prototype_GameObject_S_PaintWork"), &tPaintWorkInfo)))
			return;

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
			return;
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CJake::Control_Tick(_double TimeDelta)
{
	m_pTransformCom->Go_Straight(0, m_pNavigationCom);
	
	CObj_Manager::GetInstance()->Set_Jake_Weapon(CObj_Manager::PLAYERINFO::JAKEWEAPON::SHLDE);
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

	if (5 <= m_pModelCom->Get_Keyframes())
		m_pTransformCom->Go_Backward(0, m_pNavigationCom);
	else
		m_pTransformCom->Go_Backward(TimeDelta, m_pNavigationCom);

	if (m_pModelCom->Get_Finished())
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
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
			return;
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
	}
}

void CJake::Swim_Tick(_double TimeDelta)
{
	if (m_tPlayerInfo.ePlayer ==  CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::SWIM);

	if (!m_bDiving)
		m_pModelCom->Set_AnimIndex(43, false);	// DIVING

	if (43 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
		m_bDiving = true;

	if (m_bDiving)
	{
		m_pModelCom->Set_AnimIndex(57);			// SWIM

		// CellType 이 1 이라면 내라가다가.
		m_pTransformCom->Go_SwinDown(TimeDelta, 1.f, -0.6f);	// -0.6 변경하면 Player Follow 에서도 변경

		// CellType 이 0 이되면 올라간다.
		if (0 == m_pNavigationCom->Get_CellType())
		{
			m_pModelCom->Set_AnimIndex(53);		// RUN
			if (m_pTransformCom->Go_SwinUp(TimeDelta, 5.f))	// 0 까지 올라왔다면
			{
				m_bDiving = false;
				m_bIsSwim = false;
			}
		}
	}
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
	if (m_bIsSwim)		// 예외처리 수영 중일 때는 변하지 말기
		return S_OK;

	// m_bSkill_Clone -> ture 라면? KeyInput(), Render() 를 호출하지 않는다.
	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		// Magic 모델 생성
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CS_Change_Magic::CHANGEINFO		tChangeInfo;
		tChangeInfo.eChange = tChangeInfo.JAKE;
		tChangeInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y, f4MyPos.z);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_S_Change_Magic_JAKE"), TEXT("Prototype_GameObject_S_Change_Magic"), &tChangeInfo)))
			return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
	}

	// Magic 모델을 따라간다.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pChangeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_S_Change_Magic_JAKE"), TEXT("Com_Transform"), 0));

	if (nullptr != pChangeTransformCom)
	{
		_vector vChangePos = pChangeTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vChangePos);
	}

	m_bSkillClone_TimeAcc += TimeDelta;
	if (10 < m_bSkillClone_TimeAcc)
	{
		// Get_Dead 가 true 라면 아이들로 변경한다.
		CS_Change_Magic * pGameObject = dynamic_cast<CS_Change_Magic*>(pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, 
			TEXT("Layer_S_Change_Magic_JAKE"), TEXT("Prototype_GameObject_S_Change_Magic"), 0));
		pGameObject->Set_Dead();

		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;		// 상태 변경
		m_bSkill_Clone = false;							// 스킬 한 번만 생성되기 위해서
		
		m_bSkillClone_TimeAcc = 0;
	}

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
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

			// Level Up : 50

		case CObj_Manager::PLAYERINFO::STATE::S_COIN:
			m_pModelCom->Set_AnimIndex(25);
			break;

		case CObj_Manager::PLAYERINFO::STATE::HIT:
			m_pModelCom->Set_AnimIndex(38, false);
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
		}

		if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)	// 내가 현재 플레이어 일 때만 상태를 변경한다.
			CObj_Manager::GetInstance()->Set_Current_Player_State(m_tPlayerInfo.eState);
		m_tPlayerInfo.ePreState = m_tPlayerInfo.eState;
	}
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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
