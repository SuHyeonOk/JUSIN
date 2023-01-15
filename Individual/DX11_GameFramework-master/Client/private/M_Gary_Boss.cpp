#include "stdafx.h"
#include "..\public\M_Gary_Boss.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"

#include "B_3DBullet.h"

CM_Gary_Boss::CM_Gary_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CM_Gary_Boss::CM_Gary_Boss(const CM_Gary_Boss & rhs)
	: CGameObject(rhs)
{

}

HRESULT CM_Gary_Boss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gary_Boss::Initialize(void * pArg)
{	
	m_wsTag = L"Gary_Boss";

	_float3	f3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr != pArg)
		memcpy(&f3Pos, pArg, sizeof(_float3));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	m_eState		= IDLE;
	m_eAnimState	= IDLE;
	m_fHP			= 1000.0f;
	m_fMaxHP		= 1000.0f;
	m_fAttack		= 50.0f;
	m_fExp			= 1000.0f;

	return S_OK;
}

void CM_Gary_Boss::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Monster_Tick(TimeDelta);
	Anim_Change();
}

void CM_Gary_Boss::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CM_Gary_Boss::Render()
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
	}
#endif

	return S_OK;
}

void CM_Gary_Boss::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		//m_eState = HIT;
		//m_fHP -= CObj_Manager::GetInstance()->Get_Current_Player().fAttack;
	}
}

HRESULT CM_Gary_Boss::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_Gary"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.8f, 1.5f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gary_Boss::SetUp_ShaderResources()
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

void CM_Gary_Boss::Monster_Tick(const _double & TimeDelta)
{
	// 체력이 0 이되면 죽는다.
	if (0 >= m_fHP)
		m_eState = DIE;

	switch (m_eState)
	{
	case Client::CM_Gary_Boss::IDLE:
		// 거리 7 이내에 플레이어가 있는지 확인하고, 있으면 공격 시작
		Idle_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::MOVE:
		// 딱히 안 쓸 것 같은데. 순간이동을 해야한다.
		Move_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_BULLET:
		// 플레이어를 향한 불꽃 총알을 발사한다.
		A_Bullet_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_STUN:
		// 광역으로 플레이어를 stun 시킨다.
		A_Stun_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_CAGE:
		// 플레이어를 순간 이동 시켜서 케이지 안 에 가두고, 잡몹을 소환한다. (몬스터는 시야에 보이지 않는다.)
		A_Cage_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::A_DANCE:
		// 밴드 쪽 으로 올라가고, 아래 잡몹들이 춤을 추면 보스는 체력을 회복한다. 
		A_Dance_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::HIT:
		Hit_Tick(TimeDelta);
		break;
	case Client::CM_Gary_Boss::DIE:
		Die_Tick(TimeDelta);
		break;
	}
}

void CM_Gary_Boss::Anim_Change()
{
	// 0 : 오른손 평타
	// 1 : 왼손 평타
	// 2 : 뒤로 넘어갈랑 DIE??
	// 3 : 왼쪽 Hit
	// 4 : 손 펼락 말랑
	// 5 : IDLE
	// 6 : 춤
	// 7 : MOVE

	// 보스는 항상 플레이어를 바라본다.
	m_pTransformCom->LookAt(CObj_Manager::GetInstance()->Get_Player_Transform());

	switch (m_eAnimState)
	{
	case Client::CM_Gary_Boss::IDLE:
		m_pModelCom->Set_AnimIndex(5);
		break;
	case Client::CM_Gary_Boss::MOVE:
		m_pModelCom->Set_AnimIndex(7);
		break;
	case Client::CM_Gary_Boss::A_BULLET:
		m_pModelCom->Set_AnimIndex(0, false);
		break;
	case Client::CM_Gary_Boss::A_STUN:
		m_pModelCom->Set_AnimIndex(4, false);
		break;
	//case Client::CM_Gary_Boss::A_CAGE:
	//	break;
	case Client::CM_Gary_Boss::A_DANCE:
		m_pModelCom->Set_AnimIndex(6);
		break;
	case Client::CM_Gary_Boss::HIT:
		m_pModelCom->Set_AnimIndex(3, false);
		break;
	case Client::CM_Gary_Boss::DIE:
		m_pModelCom->Set_AnimIndex(2, false);
		break;
	}
}

void CM_Gary_Boss::Idle_Tick(const _double & TimeDelta)
{
	m_eAnimState = IDLE;

	// 일정거리 일 때만 공격한다.
	_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	//cout << fDistance << endl;
	if (7.0f < fDistance)
		return;

	m_dAttack_TimeAcc += TimeDelta;
	if (3 < m_dAttack_TimeAcc)
	{
		_int iRandom = CUtilities_Manager::GetInstance()->Get_Random(0, 1);
		iRandom = 1;
		if (0 == iRandom)
			m_eState = A_BULLET;
		else
			m_eState = A_STUN;

		m_dAttack_TimeAcc = 0;
	}
}

void CM_Gary_Boss::Move_Tick(const _double & TimeDelta)
{
}

HRESULT CM_Gary_Boss::A_Bullet_Tick(const _double & TimeDelta)
{
	if (3 <= m_iBullet_Count)
	{
		m_eState = IDLE;
		m_eAnimState = IDLE;
		m_iBullet_Count = 0;
		return S_OK;
	}

	m_eAnimState = A_BULLET;

	// 플레이어어의 좌표를 3번 받아서 따 따 따 총알을 발사한다.
	if (m_pModelCom->Get_Finished())
	{
		// 내 좌표
		_vector	vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4	f4MyPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4(&f4MyPos, vMyPos);

		// 플레이어 좌표
		_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();
		_float4	f4PlayerPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4(&f4PlayerPos, vPlayerPos);

		CB_3DBullet::NONANIMBULLETINFO	tBulletInfo;
		tBulletInfo.fMonsterAttack = m_fAttack;
		tBulletInfo.eBulletType = tBulletInfo.TYPE_ROCK;
		tBulletInfo.f3Start_Pos = _float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z);
		tBulletInfo.f3Target_Pos = _float3(f4PlayerPos.x, f4PlayerPos.y + 0.8f, f4PlayerPos.z);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_B_RandomBullet_Skeleton_0"), TEXT("Prototype_GameObject_B_RandomBullet"), &tBulletInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		RELEASE_INSTANCE(CGameInstance);

		++m_iBullet_Count;
	}

	return S_OK;
}

HRESULT CM_Gary_Boss::A_Stun_Tick(const _double & TimeDelta)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 f4MyPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4(&f4MyPos, vMyPos);

	if(true == m_bEffect_Smoke)		// 이펙트~~~
		CEffect_Manager::GetInstance()->Effect_Smoke(_float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z - 0.5f), _float3(0.0f, 0.0f, 0.0f));

	m_dStun_TimeAcc += TimeDelta;
	if (0 < m_dStun_TimeAcc)
		m_bEffect_Smoke = true;		// 이펙트 발사

	if (1 < m_dStun_TimeAcc)		
		m_pTransformCom->Set_Pos(_float3(2.43f, 0.0f, 10.75f));		// 특정 위치로 순간이동

	if (2 < m_dStun_TimeAcc)
		m_bEffect_Smoke = false;	// 이펙트 꺼
	
	if (3 < m_dStun_TimeAcc)
	{
		m_eAnimState = A_STUN;

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			cout << "생성" << endl;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_Boss_S_Scream"), &(_float3(f4MyPos.x, f4MyPos.y + 1.0f, f4MyPos.z)))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		RELEASE_INSTANCE(CGameInstance);

		m_eState = IDLE;
		m_dStun_TimeAcc = 0;
	}

	return S_OK;
}

HRESULT CM_Gary_Boss::A_Cage_Tick(const _double & TimeDelta)
{
	return S_OK;
}

HRESULT CM_Gary_Boss::A_Dance_Tick(const _double & TimeDelta)
{
	return S_OK;
}

void CM_Gary_Boss::Hit_Tick(const _double & TimeDelta)
{
	m_bShader_Hit = true;

	m_dShader_Hit_TimeAcc += TimeDelta;
	if (0.1 < m_dShader_Hit_TimeAcc)
		m_bShader_Hit = false;

	if (m_pModelCom->Get_Finished())
	{
		m_eState = IDLE;
		m_bShader_Hit = false;
	}
}

void CM_Gary_Boss::Die_Tick(const _double & TimeDelta)
{
}

CM_Gary_Boss * CM_Gary_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Gary_Boss*		pInstance = new CM_Gary_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CM_Gary_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CM_Gary_Boss::Clone(void * pArg)
{
	CM_Gary_Boss*		pInstance = new CM_Gary_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CM_Gary_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CM_Gary_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
