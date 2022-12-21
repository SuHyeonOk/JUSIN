#include "stdafx.h"
#include "..\public\Finn.h"

#include "GameInstance.h"
#include "Skill_Manager.h"	// ���� ��ų �˷���

#include "Bone.h"
#include "Finn_Weapon.h"
#include "S_Change_Magic.h"

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
	GameObjectDesc.TransformDesc.f3Pos = _float3(f3Pos.x, f3Pos.y, f3Pos.z);

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
	cout << m_pNavigationCom->Get_CellType() << endl;
	
	__super::Tick(TimeDelta);

	//Player_Info();

	//Shader_Time(TimeDelta); // Shader Hit Time

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

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//if (pGameInstance->Key_Down(DIK_P))
	//{
	//	++m_AnimiNum;
	//}
	//if (pGameInstance->Key_Down(DIK_O))
	//{
	//	--m_AnimiNum;
	//}
	//m_pModelCom->Set_AnimIndex(m_AnimiNum);
	//cout << m_AnimiNum << endl;
	//RELEASE_INSTANCE(CGameInstance);

	Parts_LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFinn::Render()
{
	// �÷��̾� ��ų �����϶� Player �� Render �� ��� ���д�.
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
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����Ѵ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

#ifdef _DEBUG
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

	m_pNavigationCom->Render();
#endif
	return S_OK;
}

void CFinn::On_Collision(CGameObject * pOther)
{
	//wstring wsMonsterTagFind = pOther->Get_Tag();
	//wstring TagTemp = wsMonsterTagFind.substr(10);

	//if (L"Monster__" == TagTemp)
	//{
	//	CObj_Manager::Set_Player_MinusHp()
	//}
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


	///* For.Lights */
	//const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;
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

void CFinn::Player_Info()
{
	//CObj_Manager::PLAYERINFO	tPlayerInfo;
	//tPlayerInfo.iHp = 

	//CObj_Manager::GetInstance()->Set_Current_Player();

	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_R))
	//{
	//	CM_PigWarrior_BEE* pObj; 
	//	// TODO : Get_Object �� ����ų�, �浹ó�� �Ͽ� ��ü�� �޾ƿ� �� ������ ����

	//	CM_Monster* pMonster = dynamic_cast<CM_Monster*>(pObj);
	//	CObj_Manager::GetInstance()->Set_Player_PushHp(pMonster->Get_Monster_Attack());
	//}

	//RELEASE_INSTANCE(CGameInstance);

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
	// ���� �÷��̾ �ƴ� ������ �ؾ��ϴ� �ൿ
	Change_Tick();
	Cheering_Tick();
	
	// ���� �÷��̾� �� �� �� �� �ൿ
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		m_tPlayerInfo.eState = CObj_Manager::GetInstance()->Get_Current_Player().eState;

	switch (m_tPlayerInfo.eState)
	{
	case CObj_Manager::PLAYERINFO::ATTACK:
		Space_Attack_Tick(TimeDelta);
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

	case CObj_Manager::PLAYERINFO::TREEWITCH:
		TreeWitch_Tick();
		break;

	case CObj_Manager::PLAYERINFO::MAGIC:
		Magic_Tick(TimeDelta);
		break;
	}

	Anim_Change(TimeDelta);
}

void CFinn::Current_Player(_double TimeDelta)
{
	if (m_tPlayerInfo.ePlayer == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)					// Player �����
	{
		CObj_Manager::GetInstance()->Tick_Player_Transform();		// ���� �÷��̾��� ��ǥ�� Tick
		Check_Follow(TimeDelta);									// ��ó�� Jake �� �ִ��� Ȯ���Ѵ�.

		// �÷��̾��� ��ų �� Ű �Է��� ���� �ʴ´�.
		if (CObj_Manager::PLAYERINFO::STATE::MAGIC != CObj_Manager::GetInstance()->Get_Current_Player().eState)
			Key_Input(TimeDelta);
		else
			m_OnMove = false;
	}
	else
	{
		Player_Follow(TimeDelta);									// Player �� ���� �ƴ϶�� ���󰣴�.								
	}
}

void CFinn::Player_Follow(_double TimeDelta)
{
	// ���� �÷��̾ ���󰣴�.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Jake ���Է�
	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);			// Jake ��ǥ �޾ƿ�

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// �� ��ǥ
	_vector		vDir = vPlayerPos - vMyPos;												// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));						// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (2.2f > fDistanceX)
		m_pTransformCom->Chase(vPlayerPos, TimeDelta * 0.5, 1.5f);
	else
		m_pTransformCom->Chase(vPlayerPos, TimeDelta, 1.5f);

	m_pTransformCom->LookAt(vPlayerPos);

	// ���� �� �ִϸ��̼�
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
	// �����ð� ���� Jake �� ��ó�� ���� �ʴٸ� Jake �� �� ��ó�� �̵���Ų��.

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), m_pTransformComTag, 0));

	_vector vPlayerPos;
	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);		// Jake ��ǥ �޾ƿ�

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	_vector		vDir = vPlayerPos - vMyPos; // �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ���� (Jaek <- Finn)

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X ���� �̾ƿ� �Ÿ� Ȯ��

	if (5.f < fDistanceX)	// �Ÿ��� 5�̻��� ��
	{
		m_dNotfollow_TimeAcc += TimeDelta;
		if (3 < m_dNotfollow_TimeAcc) // ������� �� �ϴ� �ð��� 5 �ʸ� �Ѿ�ٸ�
		{
			_vector		vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float		fLookX = XMVectorGetX(vMyLook);
			_float		fLookZ = XMVectorGetZ(vMyLook);

			_float fAddX, fAddZ;
			if (0 < fLookX)		// +
				fAddX = 1.2f;
			else				// -
				fAddX = -1.2f;

			if (0 < fLookZ)
				fAddZ = 1.2f;
			else
				fAddZ = -1.2f;

			_float4 f4MyPos;
			XMStoreFloat4(&f4MyPos, vMyPos);
			pJakeTransformCom->Set_Pos(_float3(f4MyPos.x - fAddX, f4MyPos.y, f4MyPos.z - fAddZ));	// �� ������ �ű�

			m_dNotfollow_TimeAcc = 0;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFinn::Key_Input(_double TimeDelta)
{
	if (m_tPlayerInfo.eState == m_tPlayerInfo.ROLL	||
		m_tPlayerInfo.eState == m_tPlayerInfo.HIT	||
		m_tPlayerInfo.eState == m_tPlayerInfo.STUN	||
		m_tPlayerInfo.eState == m_tPlayerInfo.TREEWITCH)
	{
		m_OnMove = false;
		return;
	}

	if (m_OnMove)
	{
		m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::RUN);
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region �̵�
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
		CObj_Manager::GetInstance()->Set_Current_Player_State(CObj_Manager::PLAYERINFO::STATE::ATTACK);

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

	//m_dHit_TimeAcc += TimeDelta;
	//if (0.3 < m_dHit_TimeAcc)
	//	m_pTransformCom->Go_Backward(0);
	//else
	//	m_pTransformCom->Go_Backward(TimeDelta);

	if (m_pModelCom->Get_Finished())
	{
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
		m_dHit_TimeAcc = 0;
	}
}

void CFinn::Stun_Tick()
{
	_vector vMyPos;
	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4	f4MyPos;
	XMStoreFloat4(&f4MyPos, vMyPos);

	if (!m_bStun)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, TEXT("S_StunChick_0"), TEXT("Prototype_GameObject_S_StunChick"), &_float3(f4MyPos.x, f4MyPos.y + 1.3f, f4MyPos.z))))
			return;
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
	}
}

void CFinn::Change_Tick()
{
	if (CObj_Manager::PLAYERINFO::STATE::CHANGE != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		return;

	m_OnMove = false;
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

void CFinn::TreeWitch_Tick()
{
	if (m_tPlayerInfo.ePlayer != CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
		return;

	// �ҸӴ� ��ų
	// [��] 55 : ������ ���� 54 : ������ 53 : �Ͼ��

	CSkill_Manager::MONSTERSKILL tMonsterSkill;
	tMonsterSkill.eTreeWitch = CSkill_Manager::GetInstance()->Get_Monster_Skill().eTreeWitch;

	if(CSkill_Manager::MONSTERSKILL::TREEWITCH::JUMP == CSkill_Manager::GetInstance()->Get_Monster_Skill().eTreeWitch)
		m_pModelCom->Set_AnimIndex(55, false);
	else if(CSkill_Manager::MONSTERSKILL::TREEWITCH::PRESSURE == CSkill_Manager::GetInstance()->Get_Monster_Skill().eTreeWitch)
		m_pModelCom->Set_AnimIndex(54, false);
	else if (CSkill_Manager::MONSTERSKILL::TREEWITCH::RISE == CSkill_Manager::GetInstance()->Get_Monster_Skill().eTreeWitch)
	{
		m_pModelCom->Set_AnimIndex(53, false);
		if (m_pModelCom->Get_Finished())
			m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;
	}
}

HRESULT CFinn::Magic_Tick(_double TimeDelta)
{
	// m_bSkill_Clone -> ture ���? KeyInput(), Render() �� ȣ������ �ʴ´�.
	if (!m_bSkill_Clone)
	{
		m_bSkill_Clone = true;

		// Magic �� ����
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		CS_Change_Magic::CHANGEINFO		tChangeInfo;
		tChangeInfo.eChange = tChangeInfo.FINN;
		tChangeInfo.f3Pos = _float3(f4MyPos.x, f4MyPos.y, f4MyPos.z);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Change_Magic_FINN"), TEXT("Prototype_GameObject_S_Change_Magic"), &tChangeInfo)))
			return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
	}

	// Magic ���� ���󰣴�.
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pChangeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_S_Change_Magic_FINN"), TEXT("Com_Transform"), 0));

	m_bSkillClone_TimeAcc += TimeDelta;
	if (nullptr == pChangeTransformCom)
	{
		// ���� �Ǿ��ٰ� �ٷ� ���� �Ϸ��� �� �� ������ �ּҸ� ã�ƿ��� �� �ϴ� ������ �־ ���� �ð��� �� ���� ���� ������ �Ѵ�.
		if (0.5 < m_bSkillClone_TimeAcc)
			_int a = 0;
	}
	else
	{
		_vector vChangePos;
		vChangePos = pChangeTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vChangePos);
		RELEASE_INSTANCE(CGameInstance);
	}

	// Get_Dead �� true ��� ���̵�� �����Ѵ�.
	if (15 < m_bSkillClone_TimeAcc)
	{
		m_tPlayerInfo.eState = m_tPlayerInfo.IDLE;	// ���� ����
		m_bSkill_Clone = false;							// ��ų �� ���� �����Ǳ� ���ؼ�
		m_bSkillClone_TimeAcc = 0;
	}

	return S_OK;
}

void CFinn::Anim_Change(_double TimeDelta)
{   
	if (m_tPlayerInfo.TREEWITCH == m_tPlayerInfo.eState)
		return;

	if (m_tPlayerInfo.ePreState != m_tPlayerInfo.eState)
	{
		switch (m_tPlayerInfo.eState)
		{
		case CObj_Manager::PLAYERINFO::STATE::IDLE:
			m_pModelCom->Set_AnimIndex(39);
			break;

		case CObj_Manager::PLAYERINFO::STATE::RUN:
			m_pModelCom->Set_AnimIndex(49);
			break;

		case CObj_Manager::PLAYERINFO::STATE::ROLL:
			m_pModelCom->Set_AnimIndex(48, false);
			break;

		case CObj_Manager::PLAYERINFO::STATE::ATTACK:
			m_pModelCom->Set_AnimIndex(5, false);
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
