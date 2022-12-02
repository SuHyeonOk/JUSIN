#include "stdafx.h"
#include "..\public\Coin.h"
#include "GameInstance.h"

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
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (nullptr != pArg)
		memcpy(&m_tinCoinInfo, pArg, sizeof(COININFO));

	if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_BRONZE)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinCoinInfo.fPos.x, m_tinCoinInfo.fPos.y, m_tinCoinInfo.fPos.z);
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_SILVER)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinCoinInfo.fPos.x, m_tinCoinInfo.fPos.y, m_tinCoinInfo.fPos.z);
	}
	else if (m_tinCoinInfo.eCoinKind == m_tCoinInfo.COIN_GOLD)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
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

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 f4Position;
	XMStoreFloat4(&f4Position, vPosition);

	cout << (_double)(rand() % 150) / 150.10 << endl;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_Y))
	{
		m_bBigJump = false;
		m_bRotation = false;

		m_bOneDit = false;
	}

	RELEASE_INSTANCE(CGameInstance);

	// Pos 중심으로 몇개를 뿌릴 것 인지 Item Manager 에서 

	// 제자리에서 점프 하면서 Rendom 으로 x, z 좌표 만큼 멀어지고, 그 자리에서 회전한다.

	// 0.5 ~ 1.5 사이
	_float fRandonNum = (_double)(rand() % 150) / 150.100;
	_float	fHight = fRandonNum;
	_float	fSpeed = 6.f;

	// 큰 점프 후 작은 점프 3번
	if (!m_bBigJump)
	{
		m_fSmallJump = 0.f;

		if (m_pTransformCom->Jump(fHight, fSpeed, TimeDelta))
			m_bBigJump = true;
	}
	else
	{
		if (fHight <= m_fSmallJump)
			m_bRotation = true; // 큰 점프 후 작은 점프 3번 후 회전
			//m_bBigJump = false; // 큰 점프 후 작은 점프 3번 반복

		if (m_pTransformCom->Jump((fHight - m_fSmallJump), (fSpeed + m_fSmallJump), TimeDelta))
			m_fSmallJump += 0.5f;
	}

	if (m_bRotation)
	{
		m_pTransformCom->Set_Pos(m_tinCoinInfo.fPos.y);
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
	}
	else
	{
		if (!m_bOneDit)
		{
			_float fRandonNum = (_float)(rand() % 360);
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(fRandonNum));

			m_bOneDit = true;
		}
		m_pTransformCom->Go_Straight(TimeDelta);
	}
}

void CCoin::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

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
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CCoin::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
