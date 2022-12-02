#include "stdafx.h"
#include "..\public\Page.h"

#include "GameInstance.h"

#include "ItemManager.h"

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

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjectDesc.TransformDesc.f3Pos = _float3(m_tinPageInfo.fPos.x, m_tinPageInfo.fPos.y, m_tinPageInfo.fPos.z);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CPage::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_U))
	{
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vMyPosf4;
		XMStoreFloat4(&vMyPosf4, vMyPos);

		CItemManager::GetInstance()->RandomCoin_Clone(_float3(vMyPosf4.x, vMyPosf4.y, vMyPosf4.z), 5, 2, 1);
	}

	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
	m_pTransformCom->Jump(0.5f, 0.3f, TimeDelta);

	// 제 자리에서 뛰었다가 회전하고 반복 그런데 자연스럽지가 않음
	//if (m_bIdle) 
	//{
	//	if (Rotation(0.3, 2, TimeDelta))
	//		m_bIdle = false;
	//}
	//else
	//{
	//	if (m_pTransformCom->Jump(1.f, 2.f, TimeDelta))
	//		m_bIdle = true;
	//}
}

void CPage::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
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
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달한다. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CPage::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
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

_bool CPage::Rotation(_double dStartTime, _double dStopTime, _double TimeDelta)
{
	// y 축을 기준으로 회전하고 있으며, dStartTime 에 시작하고, dStopTime 때 멈춘다.

	if (!m_bRotation_Stop)
	{
		m_dRotation_Start_TimeAcc += TimeDelta;
		if (dStartTime < m_dRotation_Start_TimeAcc)
		{
			m_bRotation_Start = true;
			m_bRotation_Stop = true;
			m_dRotation_Start_TimeAcc = 0;

			return true;
		}
	}

	if (m_bRotation_Start)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta * 4);

		m_dRotation_Stop_TimeAcc += TimeDelta;
		if (dStopTime < m_dRotation_Stop_TimeAcc)
		{
			m_bRotation_Start = false;
			m_bRotation_Stop = false;
			m_dRotation_Stop_TimeAcc = 0;
		}
	}

	return false;
}

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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
