#include "stdafx.h"
#include "..\public\Coin.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CCoin::CCoin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CCoin::CCoin(const CCoin & rhs)
	: CGameObject(rhs)
{

}

HRESULT CCoin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin::Initialize(void * pArg)
{
	m_wsTag = L"Item_Coin";

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (nullptr != pArg)
		memcpy(&m_tinCoinInfo, pArg, sizeof(COININFO));

	if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_BRONZE)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinCoinInfo.fPos.x, m_tinCoinInfo.fPos.y, m_tinCoinInfo.fPos.z);
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_SILVER)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinCoinInfo.fPos.x, m_tinCoinInfo.fPos.y, m_tinCoinInfo.fPos.z);
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_GOLD)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinCoinInfo.fPos.x, m_tinCoinInfo.fPos.y, m_tinCoinInfo.fPos.z);
	}

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CCoin::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CurrentState(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_ITME, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CCoin::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	//if (CObj_Manager::GetInstance()->Get_Player_Collider(&m_pColliderCom))
	//	m_bPlayer_Collider = true;

	if(m_bPlayer_Collider)
	{
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta);

		m_dDead_TimeAcc += TimeDelta;
		if (1 < m_dDead_TimeAcc)
		{
			CGameObject::Set_Dead();
			m_dDead_TimeAcc = 0;
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCoin::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (1 == i) // 그림자 모델 없앰 (비중이 클 수록 모델 순서가 앞에 위치한다.)
			continue;

		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}

	return S_OK;
}

void CCoin::On_Collision(CGameObject * pOther)
{
	if (L"Finn" == pOther->Get_Tag() || L"Jake" == pOther->Get_Tag())
		m_bPlayer_Collider = true;
}

HRESULT CCoin::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_BRONZE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Prototype_Component_Model_CoinBronze"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_SILVER)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Prototype_Component_Model_CoinSilver"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_GOLD)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(CObj_Manager::GetInstance()->Get_Current_Level(), TEXT("Prototype_Component_Model_CoinGold"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin::SetUp_ShaderResources()
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

void CCoin::CurrentState(_double TimeDelta)
{
	if (m_tinCoinInfo.eState == m_tCoinInfo.IDLE)	// 평상시에 회전하고 있다
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
	}
	else											// 동전 튀어나오는
		m_pTransformCom->RandomJump(800, 6.f, 0.5f, TimeDelta);
}

CCoin * CCoin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCoin*		pInstance = new CCoin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCoin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCoin::Clone(void * pArg)
{
	CCoin*		pInstance = new CCoin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCoin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCoin::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
