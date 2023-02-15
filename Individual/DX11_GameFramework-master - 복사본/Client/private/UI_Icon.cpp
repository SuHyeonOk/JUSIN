#include "stdafx.h"
#include "..\public\UI_Icon.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Icon::CUI_Icon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_Icon::CUI_Icon(const CUI_Icon & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_Icon::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Icon::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eIconIndex, pArg, sizeof(INVENTORYICON));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 338.0f / 1.5f;
	m_fSizeY = 61.0f / 1.5f;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.7f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	Icon_SetPos();

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_Icon::Tick(_double TimeDelta)
{
	// Jake Son 
	if(5 == CUI_Manager::GetInstance()->Get_JakeSon_Count())
		CUI_Manager::GetInstance()->Set_InventoryIcon(INVENTORYICON::ICON_ONE, CSkill_Manager::PLAYERSKILL::SKILL::JAKESON);
	else
		CUI_Manager::GetInstance()->Set_InventoryIcon(INVENTORYICON::ICON_ONE, CSkill_Manager::PLAYERSKILL::SKILL::LOOK_JAKESON);
}

void CUI_Icon::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Icon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	if (m_eIconIndex == INVENTORYICON::ICON_ONE)
	{
		if (5 == CUI_Manager::GetInstance()->Get_JakeSon_Count())
			return S_OK;

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		_tchar szCount[10];
		wsprintf(szCount, TEXT("%d/5"), CUI_Manager::GetInstance()->Get_JakeSon_Count());

		pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(548.667f, 225.467f), 0.f, _float2(0.35f, 0.33f));

		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

HRESULT CUI_Icon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Icon"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Icon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	switch (m_eIconIndex)
	{
	case Client::INVENTORYICON::ICON_ONE:
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON::ICON_ONE)))))
			return E_FAIL;
	}
		break;
	case Client::INVENTORYICON::ICON_TWO:
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON::ICON_TWO)))))
			return E_FAIL;
	}
		break;
	case Client::INVENTORYICON::ICON_THREE:
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON::ICON_THREE)))))
			return E_FAIL;
	}
		break;
	case Client::INVENTORYICON::ICON_FOUR:
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON::ICON_FOUR)))))
			return E_FAIL;
	}
		break;
	case Client::INVENTORYICON::ICON_FIVE:
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON::ICON_FIVE)))))
			return E_FAIL;
	}
		break;
	case Client::INVENTORYICON::ICON_SIX:
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", _int(CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON::ICON_SIX)))))
			return E_FAIL;
	}
		break;
	}

	return S_OK;
}

void CUI_Icon::Icon_SetPos()
{
	switch (m_eIconIndex)
	{
	case Client::INVENTORYICON::ICON_ONE:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, 126.467f, 0.f, 1.f));
		break;
	case Client::INVENTORYICON::ICON_TWO:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, 82.4667f, 0.f, 1.f));
		break;
	case Client::INVENTORYICON::ICON_THREE:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, 38.4667f, 0.f, 1.f));
		break;
	case Client::INVENTORYICON::ICON_FOUR:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, -6.53333f, 0.f, 1.f));
		break;
	case Client::INVENTORYICON::ICON_FIVE:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, -50.5333f, 0.f, 1.f));
		break;
	case Client::INVENTORYICON::ICON_SIX:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, -93.5333f, 0.f, 1.f));
		break;
	}
}

CUI_Icon * CUI_Icon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_Icon*		pInstance = new CUI_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Icon::Clone(void * pArg)
{
	CUI_Icon*		pInstance = new CUI_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
