#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	
	m_pGameInstance = GET_INSTANCE(CGameInstance);

	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//Key_Input(TimeDelta);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), 
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* For.Com_Tranform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Tranform"),
		(CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4			ViewMatrix, ProjMatrix;
	
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -30.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (_float)g_iWinSizeY, 0.2f, 300.f));

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Key_Input(_double TimeDelta)
{
	if(m_pGameInstance->Key_Pressing(DIK_UP) && m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), 45.f);
		//m_pTransformCom->Go_Backward(0.001);
	}
	else
	{
		if (m_pGameInstance->Key_Pressing(DIK_UP))
		{
			m_pTransformCom->Go_Backward(0.01);
		}
		if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		{
			m_pTransformCom->Go_Straight(0.01);
		}
		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		{
			m_pTransformCom->Go_Right(0.01);
		}
		if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			m_pTransformCom->Go_Left(0.01);
		}
	}

}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	RELEASE_INSTANCE(CGameInstance);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
