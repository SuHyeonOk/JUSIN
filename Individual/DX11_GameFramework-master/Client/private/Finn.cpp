#include "stdafx.h"
#include "..\public\Finn.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Finn_Weapon.h"

#include "ItemManager.h"
#include "Skill_Manager.h"
#include "Effect_Manager.h"

#include "S_PaintWork.h"
#include "S_Fiona.h"
#include "Player_Talk.h"

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
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);
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
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_V))
	{
		//////////////////////////// ����׿�
		_vector vddMyPos;
		vddMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float4	f4ddMyPos;
		XMStoreFloat4(&f4ddMyPos, vddMyPos);

		cout << "��" << f4ddMyPos.x << " | " << f4ddMyPos.y << " | " << f4ddMyPos.z << " || " << m_pNavigationCom->Get_CellIndex() << endl;
		//////////////////////////// ����׿�
	}

	RELEASE_INSTANCE(CGameInstance);

	if (true == CSkill_Manager::GetInstance()->Get_ChangeSkill_Create())
		return;

	__super::Tick(TimeDelta);
	
	BossCage(TimeDelta);
	Parts_Tick(TimeDelta);

	Current_Player(TimeDelta);
	Player_Tick(TimeDelta);
}

void CFinn::Late_Tick(_double TimeDelta)
{
	if (true == CSkill_Manager::GetInstance()->Get_ChangeSkill_Create())
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
			//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_XRAYBLEND, this);
		}
	}

	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);		// �浹ó��
		m_pColliderCom[COLLTYPE_AABB]->Update(m_pTransformCom->Get_WorldMatrix());
	}
}

HRESULT CFinn::Render()
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
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);			// �׸���
		else
		{
			if (m_bShader_Hit && CObj_Manager::PLAYERINFO::STATE::HIT == m_tPlayerInfo.eState)
				m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 3);		// Hit ��
			else
				if (1 != m_fAlpha)
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 2);	// ���� ��
				else
					m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");		// ���
		}
	}

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		for (_uint i = 0; i < COLLTYPE_END; ++i)
		{
			if (nullptr != m_pColliderCom[i])
				m_pColliderCom[i]->Render();
		}

		m_pNavigationCom->Render();
	}
#endif

	return S_OK;
}

HRESULT CFinn::Render_XRay()
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
		if (0 == i)
			continue;

		m_pModelCom->Bind_Material(m_pShaderXRayCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderXRayCom, i, "g_BoneMatrices", 0);
	}
	return S_OK;
}

HRESULT CFinn::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CRenderer::LIGHTDESC tLightDesc = m_pRendererCom->Get_LightDesc();

	_vector vLightEye = XMLoadFloat4(&tLightDesc.f4LightEye);
	_vector vLightAt = XMLoadFloat4(&tLightDesc.f4LightAt);
	_vector vLightUp = XMLoadFloat4(&tLightDesc.f4LightUp);

	_matrix	LightViewMatrix;
	LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4	f4LightViewMatrix;
	XMStoreFloat4x4(&f4LightViewMatrix, LightViewMatrix);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &f4LightViewMatrix)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����Ѵ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 4);
	}

	return S_OK;
}

void CFinn::On_Collision(CGameObject * pOther)
{

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

	/* For.Com_ShaderXRay */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel_XRay"), TEXT("Com_ShaderXRay"),
		(CComponent**)&m_pShaderXRayCom)))
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

HRESULT CFinn::SetUp_ShaderXRayResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderXRayCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderXRayCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_float	fObjectID = 1.0f;
	if (FAILED(m_pShaderXRayCom->Set_RawValue("g_ObjectID", &fObjectID, sizeof _float)))
		return E_FAIL;

	return S_OK;
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
	if (1 != m_fAlpha)	// ��Ȱ �ؾ� �ϴ� �ð��� ����Ѵ�.
		return;

	// ���� �÷��̾ �ƴ� ������ �ؾ��ϴ� �ൿ
	Change_Tick();
	Cheering_Tick();

	// ����!!
	if (m_bIsSwim)
	{
		Swim_Tick(TimeDelta);
		return;
	}
	else
		if (1 == m_pNavigationCom->Get_CellType())
			m_bIsSwim = true;

	// ���� �÷��̾� �� �� �� �� �ൿ	
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
	}

	Anim_Change(TimeDelta);
}

void CFinn::Current_Player(_double TimeDelta)
{
	Sound_Tick();
	
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)					// Player �����
	{
		CObj_Manager::GetInstance()->Tick_Player_Transform();		// ���� �÷��̾��� ��ǥ�� Tick
		Player_Skill_Tick(TimeDelta);
		Current_HP(TimeDelta);

		if (1 != m_fAlpha)	// ��Ȱ �ؾ� �ϴ� �ð��� ����Ѵ�.
			return;
		Key_Input(TimeDelta);
	}
	else
	{
		Talk(TimeDelta);
		Player_Follow(TimeDelta);									// Player �� ���� �ƴ϶�� ���󰣴�.								
		Check_Follow(TimeDelta);									// Player ��ó�� ���� �ִ��� Ȯ���Ѵ�. ���� : �� �ֺ� ���� �Է��Ѵ�.
	}
}

void CFinn::Player_Skill_Tick(_double TimeDelta)
{
	if (m_bIsSwim)		// ���� �� ���� ��ų ��� ����
		return;

	// ��ü������ ��ų�� on �Ѵ�.
	if (CSkill_Manager::PLAYERSKILL::PAINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill ||
		CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		m_bSkillStart = true;

	if (CSkill_Manager::PLAYERSKILL::COIN == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill ||
		CSkill_Manager::PLAYERSKILL::FOOD == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
		m_bOneSkillStart = true;

	if (m_bSkillStart)	// 20 �� ���� ����� ��ų�� �ð��� ����Ѵ�.
	{
		m_dSkill_TimeAcc += TimeDelta;	// ��ų ��� �� �����ð� �� �ʱ�ȭ
		if (20 < m_dSkill_TimeAcc)
		{
			m_bSkillStart = false;
		
			// ��� ��ų�� false �� �����Ѵ�. (���������� Ű �Է��� �ϴ� ���� �߰� ó��)
			CSkill_Manager::GetInstance()->Set_Player_Skill(CSkill_Manager::PLAYERSKILL::SKILL_END);
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
			m_bSkill_Clone = false;
			m_dSkill_TimeAcc = 0;
		}
	}

	if (m_bOneSkillStart)	// 3 �� ���� ����� ��ų�� �ð��� ����Ѵ�.
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

	// ��ų �� ���� ������ ��
	if (!m_bSkill_Clone)																							// ó�� ������! ��ü�� �������� �ʾ��� ��
	{
		if (CSkill_Manager::PLAYERSKILL::MARCELINT == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)		// �ش� ��ų��
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_MARCELINE);			// ���� ��Ų��.

		if (CSkill_Manager::PLAYERSKILL::COIN == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_COIN);

		if (CSkill_Manager::PLAYERSKILL::FOOD == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_FOOD);

		if (CSkill_Manager::PLAYERSKILL::FIONA == CSkill_Manager::GetInstance()->Get_Player_Skill().eSkill)			// ���� ��ų�� ��� �� ��ü���� ��� ���� ó���Ѵ�.
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::S_FIONA);
	}
}

void CFinn::Player_Follow(_double TimeDelta)
{
	// ���� �÷��̾�  Jake�� ���󰣴�.
	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();			// Jake ��ǥ �޾ƿ�

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// �� ��ǥ
	_vector		vDir = vPlayerPos - vMyPos;												// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));						// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (2.2f > fDistanceX)
		m_pTransformCom->Chase(vPlayerPos, TimeDelta * 0.5, 1.5f, m_pNavigationCom);
	else
		m_pTransformCom->Chase(vPlayerPos, TimeDelta, 1.5f, m_pNavigationCom);

	// ���� �� �� ���� Look �� �����ؼ� ���󰣴�. (�÷��̾ ���� �� �� ���� Look �� �Ʒ��� ���󰡼�..)
	if (CObj_Manager::PLAYERINFO::SWIM == CObj_Manager::GetInstance()->Get_Current_Player().eState)		// �÷��̾ ���� �� �ε�
	{
		_float4 f4PlayerPos;
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);
		if (1 == m_pNavigationCom->Get_CellType())
			m_pTransformCom->LookAt(XMVectorSet(f4PlayerPos.x, -0.8f, f4PlayerPos.z, f4PlayerPos.w));	// ���� ���� �� �̶�� ���� ����,
		else
			m_pTransformCom->LookAt(XMVectorSet(f4PlayerPos.x, 0.f, f4PlayerPos.z, f4PlayerPos.w));		// �� ���� ���� �ƴ϶�� 0 �� ����.
	}
	else
		m_pTransformCom->LookAt(vPlayerPos);

	// ���� �� �ִϸ��̼� (���� ��쿡�� ����ũ�� ������ ������ �� �ٸ� ���� ��� �ʿ��ϴ�.)
	if (CSkill_Manager::MAGICSKILL::SKILLSTATE::SKILLSTATE_END != CSkill_Manager::GetInstance()->Get_Magic_Skill().eSkill)
	{
		cout << "�� �ʿ���" << CSkill_Manager::GetInstance()->GetInstance()->Get_Magic_Skill().eSkill << endl;

		if (CSkill_Manager::MAGICSKILL::ATTACK == CSkill_Manager::GetInstance()->GetInstance()->Get_Magic_Skill().eSkill)
			m_tPlayerInfo.eState = m_tPlayerInfo.CHANGE;
		else if (CSkill_Manager::MAGICSKILL::RUN == CSkill_Manager::GetInstance()->GetInstance()->Get_Magic_Skill().eSkill)
			m_tPlayerInfo.eState = m_tPlayerInfo.RUN;
		else
			m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
	}
	else
	{
		if (CObj_Manager::PLAYERINFO::STATE::RUN == CObj_Manager::GetInstance()->Get_Current_Player().eState ||
			CObj_Manager::PLAYERINFO::STATE::ROLL == CObj_Manager::GetInstance()->Get_Current_Player().eState)
		{
			if (1.5f < fDistanceX)	// �Ÿ� �� �� ���� ���� ���󰡴� ���� �켱!
				m_tPlayerInfo.eState = m_tPlayerInfo.RUN;
		}
		else
		{
			if (1.5f > fDistanceX)
				m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		}
	}
}

void CFinn::Check_Follow(_double TimeDelta)
{
	// ���� �ð� ���� ���� �÷��̾ ������ ������ �÷��̾� ��ó�� �̵��Ѵ�.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	_vector		vDir = vPlayerPos - vMyPos; // �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ���� (Finn <- Jake)

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (3.f < fDistanceX)
	{
		m_dNotfollow_TimeAcc += TimeDelta;
		if (3 < m_dNotfollow_TimeAcc) // ������� �� �ϴ� �ð��� 5 �ʸ� �Ѿ�ٸ�
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

			_float4 f4Position;
			XMStoreFloat4(&f4Position, vPlayerPos);
			m_pNavigationCom->Set_CellIndex(pNavigationCom->Get_CellIndex());
			f4Position = { f4Position.x - fAddX, f4Position.y, f4Position.z - fAddZ, 1.f };
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(f4Position.x, f4Position.y, f4Position.z, f4Position.w), m_pNavigationCom);	// �÷��̾� ��ó�� �̵�

			// ����Ʈ
			CEffect_Manager::GetInstance()->Effect_Smoke_Count(_float3(f4Position.x, f4Position.y + 1.0f, f4Position.z - 0.7f), _float3(0.396f, 0.654f, 0.796f), 50, { 0.3f, 1.3f });
			CEffect_Manager::GetInstance()->Effect_Star3_Count(_float3(f4Position.x, f4Position.y + 1.0f, f4Position.z - 0.8f));

			// ����
			pGameInstance->Play_Sound(TEXT("sfx_enchiridion_splash_pie.ogg"), 0.7f);

			m_dNotfollow_TimeAcc = 0;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Key_Input(_double TimeDelta)
{
	if (true == CObj_Manager::GetInstance()->Get_Interaction() ||
		m_tPlayerInfo.eState == m_tPlayerInfo.ROLL ||
		m_tPlayerInfo.eState == m_tPlayerInfo.HIT ||
		m_tPlayerInfo.eState == m_tPlayerInfo.STUN ||
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

		m_pTransformCom->PlayerMove(XMVectorSet(m_f4NewLook.x, m_f4NewLook.y, m_f4NewLook.z, m_f4NewLook.w), TimeDelta);

		//// ��� ������ ������ �ϰ� ����� ���� Look �� ����� �� ������ �����Ѵ�.
		//_vector vCurrentLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		//_vector vNewLook = XMVectorSet(m_f4NewLook.x, m_f4NewLook.y, m_f4NewLook.z, m_f4NewLook.w);

		//_float fArithmetic = XMVectorGetX(XMVector3Dot(vCurrentLook, vNewLook));

		//if (1.0f != fArithmetic)
		//{
		//	if (1 > m_fAngle)
		//		m_fAngle += _float(TimeDelta);
		//}
		//else // if (0 == fArithmetic)
		//{
		//	m_fAngle = 0;
		//}

		//if (-1.0f == fArithmetic)
		//{
		//	m_pTransformCom->Turn({ 0.0f, 1.0f, 0.0f, 1.0f }, TimeDelta * 10.0);
		//}
		//else
		//{
		//	// LinearVector() �Լ��� �̿��Ͽ� ���� �ٶ���� �ϴ� Look �� ���� ���� m_fAngle �� ���� õõ�� �޾ƿ´�.
		//	_vector vTempLook = m_pTransformCom->LinearVector(vCurrentLook, vNewLook, m_fAngle);
		//	_vector vTempRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vTempLook);
		//	_vector vTempUp = XMVector3Cross(vTempLook, vTempRight);

		//	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vTempRight);
		//	m_pTransformCom->Set_State(CTransform::STATE_UP, vTempUp);
		//	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTempLook);
		//}
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region �̵�
	if (pGameInstance->Key_Pressing(DIK_UP))
	{
		m_bAttack_Sound = false;
		pGameInstance->Stop_Sound(1);
		pGameInstance->Stop_Sound(2);

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
		pGameInstance->Stop_Sound(2);

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
		pGameInstance->Stop_Sound(2);

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
		pGameInstance->Stop_Sound(2);

		m_OnMove = true;
		m_f4NewLook = { -1.0f, 0.0f, 0.0f, 0.0f };

		if (pGameInstance->Key_Pressing(DIK_UP))
			m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
		if (pGameInstance->Key_Pressing(DIK_DOWN))
			m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	}

	//if (pGameInstance->Key_Pressing(DIK_UP))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { 0.0f, 0.0f, 1.0f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_UP) && pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { 0.5f, 0.0f, 0.5f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { 1.0f, 0.0f, 0.0f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_RIGHT) && pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { 0.5f, 0.0f, -0.5f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { 0.0f, 0.0f, -1.0f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_DOWN) && pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { -0.5f, 0.0f, -0.5f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { -1.0f, 0.0f, 0.0f, 0.0f };
	//}
	//if (pGameInstance->Key_Pressing(DIK_UP) && pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_f4NewLook = { -0.5f, 0.0f, 0.5f, 0.0f };
	//}

	//if (pGameInstance->Key_Pressing(DIK_UP))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_UP) && pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_RIGHT) && pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_DOWN) && pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(270.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_UP) && pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
	//}


	//if (pGameInstance->Key_Pressing(DIK_UP))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));

	//	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));
	//	if (pGameInstance->Key_Pressing(DIK_LEFT))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_RIGHT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

	//	if (pGameInstance->Key_Pressing(DIK_UP))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(45.f));
	//	if (pGameInstance->Key_Pressing(DIK_DOWN))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));

	//	if (pGameInstance->Key_Pressing(DIK_RIGHT))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
	//	if (pGameInstance->Key_Pressing(DIK_LEFT))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	//}
	//if (pGameInstance->Key_Pressing(DIK_LEFT))
	//{
	//	m_OnMove = true;
	//	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(270.f));

	//	if (pGameInstance->Key_Pressing(DIK_UP))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(315.f));
	//	if (pGameInstance->Key_Pressing(DIK_DOWN))
	//		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(225.f));
	//}
#pragma endregion

	if (pGameInstance->Key_Up(DIK_UP) || pGameInstance->Key_Up(DIK_RIGHT) || pGameInstance->Key_Up(DIK_DOWN) || pGameInstance->Key_Up(DIK_LEFT))
	{
		m_OnMove = false;
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
	}

	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_bSkillStart &&
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

				pGameInstance->Play_Sound(TEXT("Finn_Attack.mp3"), 0.7f, false, 1);
				pGameInstance->Play_Sound(TEXT("Finn_Sword.mp3"), 0.7f, false, 2);
			}
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);
		}
	}

	if (pGameInstance->Key_Down(DIK_LSHIFT))
	{
		pGameInstance->Stop_Sound(1);
		pGameInstance->Stop_Sound(2);
		pGameInstance->Play_Sound(TEXT("roll.ogg"), 0.7f);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ROLL);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Space_Attack_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
	{
		m_bAttack_Sound = false;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance); 
		pGameInstance->Stop_Sound(1);
		RELEASE_INSTANCE(CGameInstance);

		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
	}
}

void CFinn::Attack_Paint_Tick(_double TimeDelta)
{
	m_OnMove = false;

	if (m_pModelCom->Get_Finished())
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;

	if (18 == m_pModelCom->Get_AnimIndex() && 0 <= m_pModelCom->Get_Keyframes())
		return;

	// 1) �� ��ų�� ����Ǿ�� �ϴ� �ð��� üũ�ϴ� Tick
	// 2) �����̽��ٸ� ������ �� �� ���� ��ü�� �����Ǿ�� �ϴ� Tick

	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// ��ų ����
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CS_PaintWork::PAINTWORKINFO		tPaintWorkInfo;

		_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(-15.f));
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vLook = XMVector4Transform(vLook, RotationMatrix);		// ȸ������� look �� �����´�.
		XMStoreFloat4(&tPaintWorkInfo.f4Look, vLook);			// �ѱ��.

		tPaintWorkInfo.fAttack = CObj_Manager::GetInstance()->Get_Player_Attack();	// ���ݷ��� �Ϻη� �� ���� �ѱ��.
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
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CFinn::Skill_Coin_Tick(_double TimeDelta)
{
	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;
	
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);
		CItemManager::GetInstance()->RandomCoin_Clone(_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z), 5, 5, 12); 	// ���� ����
	}
}

void CFinn::Skill_Food_Tick(_double TimeDelta)
{	
	if (false == m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		// �� Look �� �������� �Ÿ� 1��ŭ�� �� �� ��ġ�� ���� ��Ű�� �ʹ�.
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		XMVector3Normalize(vLook * 1.0f);
		_float4 f4Look;
		XMStoreFloat4(&f4Look, vLook);
		CItemManager::GetInstance()->Food_Clone(_float3(f4MyPos.x + f4Look.x, f4MyPos.y + f4Look.y, f4MyPos.z + f4Look.z));
	}
}

HRESULT CFinn::Skill_Fiona_Tick(_double TimeDelta)
{
	// �� ����
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CObj_Manager::GetInstance()->Get_Current_Level(),
		TEXT("Layer_S_Fiona"), TEXT("Prototype_GameObject_S_Fiona"), &_float3(f4MyPos.x, f4MyPos.y, f4MyPos.z))))	// ��ų ��ü�� �����Ѵ�.
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	CSkill_Manager::GetInstance()->Set_ChangeSkill_Create(true);

	return S_OK;
}

void CFinn::Roolling_Tick(_double TimeDelta)
{
	if (m_tPlayerInfo.eState == m_tPlayerInfo.STUN)
		return;

	m_OnMove = false;	// �̵� ������ shift ������ ��� �����ؼ� flase �� �ٲ�

	if (!m_pModelCom->Get_Finished())
		m_pTransformCom->Go_Straight(TimeDelta, 4.f, m_pNavigationCom);
	else
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
}

void CFinn::Hit_Tick(_double TimeDelta)
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
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CFinn::KnockBack_Hit_Tick(_double TimeDelta)
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

void CFinn::Stun_Tick()
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	if (!m_bStun)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("S_StunChick_0"), TEXT("Prototype_GameObject_S_StunChick"), &_float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);

		m_bStun = true;
	}

	if (m_pModelCom->Get_Finished())
		++m_iStun_Count;

	if (2 <= m_iStun_Count)		// �ִϸ��̼� �� �� ��� �� ��
	{
		m_bStun = false;
		m_iStun_Count = 0;
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		CObj_Manager::GetInstance()->Set_Interaction(false);
	}
}

void CFinn::Swim_Tick(_double TimeDelta)
{
	CObj_Manager::GetInstance()->Set_Interaction(false);	// ������ �� �� ���� ��ų ���ϱ� ���ؼ� ����.

	// ���� ����Ʈ
	m_dEffect_Swim_TimeAcc += TimeDelta;
	if (0.5 < m_dEffect_Swim_TimeAcc)
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CEffect_Manager::GetInstance()->Effect_Swim_Create(_float3(f4MyPos.x, 0.6f, f4MyPos.z));
		m_dEffect_Swim_TimeAcc = 0;
	}

	// ���� ����
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::SWIM);

	if (!m_bDiving)
		m_pModelCom->Set_AnimIndex(40, false);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	static _bool bSound;
	if (false == bSound)
	{
		bSound = true;
		pGameInstance->Play_Sound(TEXT("v_Finn_OutOfWater.ogg"), 1.0f);
	}

	if (40 == m_pModelCom->Get_AnimIndex() && m_pModelCom->Get_Finished())
	{
		m_bDiving = true;
	}

	if (m_bDiving)
	{
		static _bool bSounding;
		if (false == bSounding)
		{
			bSounding = true;
			pGameInstance->Play_Sound(TEXT("sfx_character_drowning.ogg"), 1.0f, true, 4);
		}

		m_pModelCom->Set_AnimIndex(52);

		// CellType �� 1 �̶�� �������ٰ�
		m_pTransformCom->Go_SwinDown(TimeDelta * 2.2f, -0.8f);	// -0.6 �����ϸ� Player Flolow() ������ ����
																// CellType �� 0 �̵Ǹ� �ö󰣴�.
		if (0 == m_pNavigationCom->Get_CellType())
		{
			m_pModelCom->Set_AnimIndex(49);
			if (m_pTransformCom->Go_SwinUp(TimeDelta, 5.f))	// 0 ���� �ö�Դٸ�
			{
				bSound = false;
				bSounding = false;
				pGameInstance->Stop_Sound(4);
				pGameInstance->Play_Sound(TEXT("sfx_character_splah.ogg"), 1.0f);

				m_bDiving = false;
				m_bIsSwim = false;
				CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::IDLE);
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
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

void CFinn::BossCage(const _double & TimeDelta)
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
			m_pTransformCom->Go_Backward(TimeDelta * 0.1, m_pNavigationCom);
	}
	else
		m_dBossCage_TimeAcc = 0;
}

void CFinn::Current_HP(const _double & TimeDelta)
{
	m_bShader_Hit = false;
	static _bool	bRevive;

	if (0 >= CObj_Manager::GetInstance()->Get_Current_Player().fHP)
	{
		m_pModelCom->Set_AnimIndex(42, false);

		if (1 == m_fAlpha)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Play_Sound(TEXT("v_Finn_Dying.ogg"), 1.0f);
			pGameInstance->Play_Sound(TEXT("v_Jake_GameOver_Finn.ogg"), 1.0f);
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

		m_pModelCom->Set_AnimIndex(41, false);

		if (1 > m_fAlpha)
			m_fAlpha += _float(TimeDelta) * 1.5f;
		
		if (1 < m_fAlpha)
		{
			m_fAlpha = 1;
			bRevive = false;
			CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::IDLE);
			CObj_Manager::GetInstance()->Set_Interaction(false);	// ���� �޴ٰ� �ױ� ������ false �� ������ �ش�.
		}
	}
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

		case CObj_Manager::PLAYERINFO::STATE::KNOCKBACKHIT:
			m_pModelCom->Set_AnimIndex(46, false);
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

		case CObj_Manager::PLAYERINFO::STATE::LEVEL_UP:
			m_pModelCom->Set_AnimIndex(44, true, false);
			break;
		}

		if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
			CObj_Manager::GetInstance()->Set_Current_Player_State(m_tPlayerInfo.eState);
		m_tPlayerInfo.ePreState = m_tPlayerInfo.eState;
	}
}

HRESULT CFinn::Talk(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Key_Down(DIK_C))
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CPlayer_Talk::PLAYERTALKINFO	 tPlayerInfo;
		tPlayerInfo.eTextureType = CPlayer_Talk::PLAYERTALKINFO::TYPE::FINN;
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
		tPlayerInfo.eTextureType = CPlayer_Talk::PLAYERTALKINFO::TYPE::FINN;
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

void CFinn::Sound_Tick()
{
	//if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)
	//{
	//	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//	pGameInstance->Stop_Sound(1);
	//	//pGameInstance->Stop_Sound(2);
	//	RELEASE_INSTANCE(CGameInstance);
	//}
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

	Safe_Release(m_pShaderXRayCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}