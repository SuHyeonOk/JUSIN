#include "stdafx.h"
#include "..\public\Jake_Change.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Skill_Manager.h"

CJake_Change::CJake_Change(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CJake_Change::CJake_Change(const CJake_Change & rhs)
	: CGameObject(rhs)
{

}

HRESULT CJake_Change::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CJake_Change::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tChangeInfo, pArg, sizeof(CHANGEINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tChangeInfo.f3Pos.x, m_tChangeInfo.f3Pos.y, m_tChangeInfo.f3Pos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (CHANGEINFO::CHANGE::FINN == m_tChangeInfo.eChange)
		m_wsTag = L"Finn_Magic";
	else if (CHANGEINFO::CHANGE::JAKE == m_tChangeInfo.eChange)
		m_wsTag = L"Jake_Magic";

	CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

void CJake_Change::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	KeyInput(TimeDelta);
	Skill_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_PLAYER, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CJake_Change::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	// 만약 플레이어의 상태가 MAGIC 가 아니라면 사라진다.
	if (CObj_Manager::PLAYERINFO::STATE::MAGIC != CObj_Manager::GetInstance()->Get_Current_Player().eState)
		CGameObject::Set_Dead();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CJake_Change::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices");
	}

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

	m_pNavigationCom->Render();
#endif
	return S_OK;
}

void CJake_Change::On_Collision(CGameObject * pOther)
{

}

HRESULT CJake_Change::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	if (CHANGEINFO::CHANGE::FINN == m_tChangeInfo.eChange)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Magic_Man_Finn"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_SPHERE */
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	}
	else if (CHANGEINFO::CHANGE::JAKE == m_tChangeInfo.eChange)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Magic_Man_Jake"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_SPHERE */
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 1.3f);
	}

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


	return S_OK;
}

HRESULT CJake_Change::SetUp_ShaderResources()
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

void CJake_Change::Skill_Tick(const _double & TimeDelta)
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
		Hit_Tick();
		break;
	}
}

void CJake_Change::Attack_Tick()
{
	if (m_pModelCom->Get_Finished())
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);
}

void CJake_Change::Hit_Tick()
{
	if (m_pModelCom->Get_Finished())
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::IDLE);
}

void CJake_Change::KeyInput(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_OnMove)
	{
		CSkill_Manager::GetInstance()->Set_Magic_Skill(CSkill_Manager::MAGICSKILL::RUN);
	}

	if (m_OnMove && 15 <= m_pModelCom->Get_Keyframes())
		m_pTransformCom->Go_Straight(TimeDelta/*, m_pNavigationCom*/);
	else if (m_OnMove && 15 >= m_pModelCom->Get_Keyframes())
		m_pTransformCom->Go_Straight(TimeDelta * 0.5/*, m_pNavigationCom*/);
		
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

CJake_Change * CJake_Change::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CJake_Change*		pInstance = new CJake_Change(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CJake_Change");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJake_Change::Clone(void * pArg)
{
	CJake_Change*		pInstance = new CJake_Change(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CJake_Change");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJake_Change::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
