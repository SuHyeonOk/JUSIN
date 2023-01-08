#include "stdafx.h"
#include "..\public\E_DieCenter.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "PipeLine.h"
#include "Utilities_Manager.h"

CE_DieCenter::CE_DieCenter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CE_DieCenter::CE_DieCenter(const CE_DieCenter & rhs)
	: CGameObject(rhs)
{

}

HRESULT CE_DieCenter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_DieCenter::Initialize(void * pArg)
{	
	m_wsTag = L"Effect_DieCenter";

	if (nullptr != pArg)
		memcpy(&m_tDieCenterInfo, pArg, sizeof(DIECENTERINFO));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.f3Pos = m_tDieCenterInfo.f3Pos;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

 	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_float fRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 0.7f);

	m_fSizeX = fRandomNumber;
	m_fSizeY = fRandomNumber;
	
	m_pTransformCom->Set_Pos();
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	return S_OK;
}

void CE_DieCenter::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform * pCameraTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	_vector vCameraPos = pCameraTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->LookAt(vCameraPos, true);		// 카메라를 바라본다.
}

void CE_DieCenter::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (0 >= m_fAlpha)
		CGameObject::Set_Dead();	// 알파값이 다 사라지면 죽음

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (0.01 < m_fAlpha)
	{
		// 랜덤하게 사이즈가 커지고, 작아지기를 위해서
		_int iRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(0, 2);

 		if (0 == iRandomNumber)
		{
			// 스케일
			m_fSizeX += _float(TimeDelta) * 3.f;
			m_fSizeY += _float(TimeDelta) * 3.f;
		}

		// 셰이더
		m_fAlpha -= _float(TimeDelta);
	}

	//m_dAlpha_TimeAcc += TimeDelta;	
	//if (0.01 < m_dAlpha_TimeAcc)
	//{
	//	// 랜덤하게 사이즈가 커지고, 작아지기를 위해서
	//	_int iRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(0, 2);

	//	if (0 == iRandomNumber)
	//	{
	//		// 스케일
	//		m_fSizeX += 0.03f;
	//		m_fSizeY += 0.03f;
	//	}

	//	// 셰이더
	//	m_fAlpha -= 0.01f;
	//	m_dAlpha_TimeAcc = 0;
	//}

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CE_DieCenter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CE_DieCenter::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_DieCenter"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_DieCenter::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(m_tDieCenterInfo.eMonsterKind))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	return S_OK;
}

CE_DieCenter * CE_DieCenter::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_DieCenter*		pInstance = new CE_DieCenter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CE_DieCenter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_DieCenter::Clone(void * pArg)
{
	CE_DieCenter*		pInstance = new CE_DieCenter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CE_DieCenter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_DieCenter::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
