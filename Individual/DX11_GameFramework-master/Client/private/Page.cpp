#include "stdafx.h"
#include "..\public\Page.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CPage::CPage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CPage::CPage(const CPage & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPage::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (nullptr != pArg)
		memcpy(&m_tinPageInfo, pArg, sizeof(PAGEINFO));
	
	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinPageInfo.fPos.x, m_tinPageInfo.fPos.y, m_tinPageInfo.fPos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();
	m_pModelCom->Set_AnimIndex(0);

	//if (CSkill_Manager::PLAYERSKILL::SKILL::PAINT == m_tinPageInfo.ePlayerSkill)
	//	m_wsTag = L"Item_Page_Paint";
	//else if (CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT == m_tinPageInfo.ePlayerSkill)
	//	m_wsTag = L"Item_Page_Marcelint";
	//else if (CSkill_Manager::PLAYERSKILL::SKILL::COIN == m_tinPageInfo.ePlayerSkill)
	//	m_wsTag = L"Item_Page_Coin";
	//else if (CSkill_Manager::PLAYERSKILL::SKILL::FIONA == m_tinPageInfo.ePlayerSkill)
	//	m_wsTag = L"Item_Page_Fiona";
	//else
	//	m_wsTag = L"Item_Page_End";

	return S_OK;
}

void CPage::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// 만약 점프 상태라면 뛰어서 떨어져야 함
	if (true == m_tinPageInfo.bJemp)
	{
		m_pTransformCom->RandomJump(600, 6.f, 0.5f, TimeDelta);
	
		//// 플레이어와의 거리가 완전 가까우면 무조건 삭제
		//_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		//if (0.5 > fDistance)
		//	CGameObject::Set_Dead();

		//// 시간 지나면 삭제
		//m_dDead_TimeAcc += TimeDelta;
		//if (2 < m_dDead_TimeAcc)
		//{
		//	CGameObject::Set_Dead();
		//	m_dDead_TimeAcc = 0;
		//}
	}

	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
	//m_pTransformCom->Jump(0.5f, 0.3f, TimeDelta);
}

void CPage::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	if (!m_bPlayer_Collider)
	{
		CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_ITME, this);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CPage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (i == 1)
			m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 1);
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

void CPage::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
	{
		// Page 가 바닥에 있을 때만 삭제된다.
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos = { 0.0f, 0.0f, 0.0f, 0.0f };
		XMStoreFloat4(&f4MyPos, vMyPos);

		if (0 == f4MyPos.y)
		{
			// 충돌한 객체의 스킬 종류를 스킬매니저에 데이터를 넘긴다.
			if (CSkill_Manager::PLAYERSKILL::SKILL::PAINT == m_tinPageInfo.ePlayerSkill)
				CSkill_Manager::GetInstance()->Page_PickUp(CSkill_Manager::PLAYERSKILL::SKILL::PAINT);
			else if (CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT == m_tinPageInfo.ePlayerSkill)
				CSkill_Manager::GetInstance()->Page_PickUp(CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT);
			else if (CSkill_Manager::PLAYERSKILL::SKILL::COIN == m_tinPageInfo.ePlayerSkill)
				CSkill_Manager::GetInstance()->Page_PickUp(CSkill_Manager::PLAYERSKILL::SKILL::COIN);
			else if (CSkill_Manager::PLAYERSKILL::SKILL::FIONA == m_tinPageInfo.ePlayerSkill)
				CSkill_Manager::GetInstance()->Page_PickUp(CSkill_Manager::PLAYERSKILL::SKILL::FIONA);

			CGameObject::Set_Dead();
		}
	}





	if (false == m_tinPageInfo.bJemp)
	{
		
			
	}
	else
		m_bPlayer_Collider = true;

}

HRESULT CPage::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (CSkill_Manager::PLAYERSKILL::SKILL::PAINT == m_tinPageInfo.ePlayerSkill)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT == m_tinPageInfo.ePlayerSkill)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_3"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (CSkill_Manager::PLAYERSKILL::SKILL::COIN == m_tinPageInfo.ePlayerSkill)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_4"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (CSkill_Manager::PLAYERSKILL::SKILL::FIONA == m_tinPageInfo.ePlayerSkill)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_5"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPage::SetUp_ShaderResources()
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

//_bool CPage::Rotation(_double dStartTime, _double dStopTime, _double TimeDelta)
//{
//	// y 축을 기준으로 회전하고 있으며, dStartTime 에 시작하고, dStopTime 때 멈춘다.
//
//	if (!m_bRotation_Stop)
//	{
//		m_dRotation_Start_TimeAcc += TimeDelta;
//		if (dStartTime < m_dRotation_Start_TimeAcc)
//		{
//			m_bRotation_Start = true;
//			m_bRotation_Stop = true;
//			m_dRotation_Start_TimeAcc = 0;
//
//			return true;
//		}
//	}
//
//	if (m_bRotation_Start)
//	{
//		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta * 4);
//
//		m_dRotation_Stop_TimeAcc += TimeDelta;
//		if (dStopTime < m_dRotation_Stop_TimeAcc)
//		{
//			m_bRotation_Start = false;
//			m_bRotation_Stop = false;
//			m_dRotation_Stop_TimeAcc = 0;
//		}
//	}
//
//	return false;
//}

CPage * CPage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPage*		pInstance = new CPage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPage::Clone(void * pArg)
{
	CPage*		pInstance = new CPage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPage::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
