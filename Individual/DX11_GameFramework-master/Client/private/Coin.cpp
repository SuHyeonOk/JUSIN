#include "stdafx.h"
#include "..\public\Coin.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

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
		memcpy(&m_tinCoinInfo, pArg, sizeof(m_tinCoinInfo));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.f);
	GameObjectDesc.TransformDesc.f3Pos = m_tinCoinInfo.fPos;

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

	CurrentState(TimeDelta);	// 동전 상태
	Collision(TimeDelta);		// 플레이어와 충돌한 뒤 상태
}

void CCoin::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_ITME, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom &&
		true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 1.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	RELEASE_INSTANCE(CGameInstance)
}

HRESULT CCoin::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pModelCom->Bind_Material(m_pShaderCom, 0, aiTextureType_DIFFUSE, "g_DiffuseTexture");
	m_pModelCom->Render(m_pShaderCom, 0);

#ifdef _DEBUG
	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();
	}
#endif

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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_BRONZE)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinBronze"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_SILVER)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinSilver"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_GOLD)
	{
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinGold"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

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

void CCoin::CurrentState(const _double & TimeDelta)
{
	if (m_tinCoinInfo.eState == m_tCoinInfo.IDLE)	// 평상시에 회전하고 있다
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
	}
	else											// 동전 튀어나오는
	{
		Random_Jump(TimeDelta);
	}
}

void CCoin::Random_Jump(const _double & TimeDelta)
{
	_float fRandonHight = CUtilities_Manager::GetInstance()->Get_Random(0.6f, 7.0f);
	_float fSpeed = CUtilities_Manager::GetInstance()->Get_Random(6.0, 8.0f);
	_float fminusHeight = CUtilities_Manager::GetInstance()->Get_Random(0.3f, 0.6f);

	if (!m_bBigJump) // 큰 점프
	{
		m_fSmallJump = 0.f;

		if (m_pTransformCom->Jump(fRandonHight, fSpeed, TimeDelta))
			m_bBigJump = true;
	}
	else // 작은 점프
	{
		if (fRandonHight <= m_fSmallJump)
			m_bRotation = true; // 큰 점프 후 작은 점프 3번 후 회전

		if (m_pTransformCom->Jump((fRandonHight - m_fSmallJump), (fSpeed + m_fSmallJump), TimeDelta))
			m_fSmallJump += fminusHeight;
	}

	if (!m_bRotation)
	{
		if (!m_bOneDir)
		{
			m_bOneDir = true;

			_float fRandonRot = (_float)(rand() % 360);
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(fRandonRot));
		}
		m_pTransformCom->Go_Straight(TimeDelta);
	}
	else
	{
		m_pTransformCom->Set_Pos(0.0f);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
	}
}

void CCoin::Collision(const _double & TimeDelta)
{
	if (m_bPlayer_Collider)	// 충돌 하면 일정시간 후 삭제
	{
		m_pTransformCom->Chase(CObj_Manager::GetInstance()->Get_Player_Transform(), TimeDelta);

		// 시간 지나면 삭제
		m_dDead_TimeAcc += TimeDelta;
		if (0.5 < m_dDead_TimeAcc)
		{
			// 플레이어와의 거리가 완전 가까우면 무조건 삭제
			_float fDistance = CObj_Manager::GetInstance()->Get_Player_Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			if (0.7 > fDistance)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
				if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_BRONZE)
				{
					CObj_Manager::GetInstance()->Set_Coin(1);
					pGameInstance->Play_Sound(TEXT("Coin1.ogg"), 0.7f);
				}
				else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_SILVER)
				{
					CObj_Manager::GetInstance()->Set_Coin(2);
					pGameInstance->Play_Sound(TEXT("Coin2.ogg"), 0.7f);
				}
				else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_GOLD)
				{
					CObj_Manager::GetInstance()->Set_Coin(5);
					pGameInstance->Play_Sound(TEXT("Coin3.ogg"), 0.7f);
				}
				RELEASE_INSTANCE(CGameInstance);

				CGameObject::Set_Dead();
			}
		}
		else if (1 < m_dDead_TimeAcc)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_BRONZE)
			{
				CObj_Manager::GetInstance()->Set_Coin(1);
				pGameInstance->Play_Sound(TEXT("Coin1.ogg"), 0.7f);
			}
			else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_SILVER)
			{
				CObj_Manager::GetInstance()->Set_Coin(2);
				pGameInstance->Play_Sound(TEXT("Coin2.ogg"), 0.7f);
			}
			else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_GOLD)
			{
				CObj_Manager::GetInstance()->Set_Coin(5);
				pGameInstance->Play_Sound(TEXT("Coin3.ogg"), 0.7f);
			}
			RELEASE_INSTANCE(CGameInstance);


			CGameObject::Set_Dead();
			m_dDead_TimeAcc = 0;
		}
	}
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
