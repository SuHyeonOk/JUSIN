#include "stdafx.h"
#include "..\public\S_Jake_Son_Transform.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "Skill_Manager.h"
#include "Effect_Manager.h"

#include "S_Jake_Son_Twister.h"

CS_Jake_Son_Transform::CS_Jake_Son_Transform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CS_Jake_Son_Transform::CS_Jake_Son_Transform(const CS_Jake_Son_Transform & rhs)
	: CGameObject(rhs)
{

}

HRESULT CS_Jake_Son_Transform::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CS_Jake_Son_Transform::Initialize(void * pArg)
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
	
	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(2);

	CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_wsTag = L"Jake";
	m_pJake_NavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));
	m_pJake_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));

	m_pBoss_TransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_Gary_Boss"), TEXT("Com_Transform"), 0));

	m_pNavigationCom->Set_CellIndex(m_pJake_NavigationCom->Get_CellIndex());	// 현재 플레이어의 네비를 넣어준다. (한 번)

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CS_Jake_Son_Transform::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//Effect_Create(TimeDelta);

	// 계속 제이크의 좌표와 네비를 변경해 준다.
	m_pJake_TransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_pJake_NavigationCom->Set_CellIndex(m_pNavigationCom->Get_CellIndex());

	JakeSon_Tick(TimeDelta);
	State_Tick(TimeDelta);
}

void CS_Jake_Son_Transform::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (STATE::SKILL == m_eState)
		return;

	m_pModelCom->Play_Animation(TimeDelta);

	if (STATE::ATTACK == m_eState)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_P_WEAPON, this);		// 충돌처리
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CS_Jake_Son_Transform::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 2)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
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

void CS_Jake_Son_Transform::On_Collision(CGameObject * pOther)
{
	if (L"Gary_Boss" == pOther->Get_Tag())
		m_bCollide = true;
}

HRESULT CS_Jake_Son_Transform::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_S_JakeSonsTransform"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 1.2f, 1.2f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */ 
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	// TODO 전체적으로 플레이 하면서 네비 잘 타지는지 확인해야 한다.
	//m_pNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton_Boss.txt"));

	return S_OK;
}

HRESULT CS_Jake_Son_Transform::SetUp_ShaderResources()
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

void CS_Jake_Son_Transform::JakeSon_Tick(const _double & TimeDelta)
{
	// 평소에는 플레이어를 따라 다니다가
	// 일정 범위 안에 몬스터가 있다면 몬스터를 공격한다.

	if (true == m_bIsSkill)
		return;

	m_dSkill_TimeAcc += TimeDelta;
	if (2.0 < m_dSkill_TimeAcc)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vBossPos = m_pBoss_TransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_vector vDistance = vBossPos - vPos;
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (4.0f > fDistance)
		{
			m_bIsSkill = true;
			m_eState = CS_Jake_Son_Transform::STATE(CUtilities_Manager::GetInstance()->Get_Random(2, 3));	// 랜덤으로 스킬을 변경한다.
		}
		else
			m_eState = RUN;


		m_dSkill_TimeAcc = 0;
	}
	else
		m_eState = RUN;
}

void CS_Jake_Son_Transform::State_Tick(const _double & TimeDelta)
{
	switch (m_eState)
	{
	case Client::CS_Jake_Son_Transform::RUN:
		Player_Follow(TimeDelta);
		break;
	case Client::CS_Jake_Son_Transform::ATTACK:
		m_pModelCom->Set_AnimIndex(1, false);
		Attack_Tick(TimeDelta);
		break;
	case Client::CS_Jake_Son_Transform::SKILL:
		m_pModelCom->Set_AnimIndex(0);
		Skill_Tick(TimeDelta);
		break;
	}
}

void CS_Jake_Son_Transform::Player_Follow(const _double & TimeDelta)
{
	_vector vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();		// Finn 좌표 받아옴

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos;											// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	// 따라가는 속도 조절
	if (2.2f > fDistanceX)
		m_pTransformCom->Chase(vPlayerPos, TimeDelta * 0.5, 1.5f, m_pNavigationCom);
	else
		m_pTransformCom->Chase(vPlayerPos, TimeDelta, 1.5f, m_pNavigationCom);

	// 계속 플레이어 바라보기
	m_pTransformCom->LookAt(vPlayerPos);

	// 애니메이션 변경
	if (1.5f < fDistanceX)
		m_pModelCom->Set_AnimIndex(3);
	if (1.5f > fDistanceX)
		m_pModelCom->Set_AnimIndex(2);
}

void CS_Jake_Son_Transform::Attack_Tick(const _double & TimeDelta)
{
	m_pTransformCom->LookAt(m_pBoss_TransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_pTransformCom->Chase(m_pBoss_TransformCom->Get_State(CTransform::STATE_TRANSLATION), TimeDelta, 0.7f);

	if (m_pModelCom->Get_Finished() && true == m_bCollide)
	{
		m_bCollide = false;
		m_bIsSkill = false;
		m_eState = RUN;
	}
}

HRESULT CS_Jake_Son_Transform::Skill_Tick(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bCreate)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4Pos = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4(&f4Pos, vPos);

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, TEXT("Layer_Jake_Son_Twister"), TEXT("Prototype_GameObject_S_Jake_Son_Twister"), &_float3(f4Pos.x, f4Pos.y, f4Pos.z))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		m_bCreate = true;
	}

	CS_Jake_Son_Twister * pGameObject = dynamic_cast<CS_Jake_Son_Twister*>(pGameInstance->Get_GameObjectPtr(LEVEL_SKELETON_BOSS, TEXT("Layer_Jake_Son_Twister"), TEXT("Prototype_GameObject_S_Jake_Son_Twister"), 0));
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == pGameObject)
	{
		m_bCreate = false;
		m_bIsSkill = false;
		m_eState = RUN;
	}

	return S_OK;
}

CS_Jake_Son_Transform * CS_Jake_Son_Transform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CS_Jake_Son_Transform*		pInstance = new CS_Jake_Son_Transform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CS_Jake_Son_Transform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CS_Jake_Son_Transform::Clone(void * pArg)
{
	CS_Jake_Son_Transform*		pInstance = new CS_Jake_Son_Transform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CS_Jake_Son_Transform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CS_Jake_Son_Transform::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
