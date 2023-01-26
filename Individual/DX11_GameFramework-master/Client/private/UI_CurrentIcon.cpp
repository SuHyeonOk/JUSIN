#include "stdafx.h"
#include "..\public\UI_CurrentIcon.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_CurrentIcon::CUI_CurrentIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_CurrentIcon::CUI_CurrentIcon(const CUI_CurrentIcon & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_CurrentIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_CurrentIcon::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 338.0f / 1.5f;
	m_fSizeY = 70.0f / 1.5f;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.7f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.0f, m_fCurrentIndexY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_CurrentIcon::Tick(_double TimeDelta)
{
	//static _float fCurrentIndexY = 123.0f;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_RIGHT) && 12 > m_iCurrentIcon_Index)	// 아래
	{
		m_fCurrentIndexY -= 44.333f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.0f, m_fCurrentIndexY, 0.f, 1.f));
	
		++m_iCurrentIcon_Index;
		m_eCurrentIcon_Index = INVENTORYICON(m_iCurrentIcon_Index);
	}

	if (pGameInstance->Key_Down(DIK_LEFT) && 0 < m_iCurrentIcon_Index)	// 위								
	{																			
		m_fCurrentIndexY += 44.333f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.0f, m_fCurrentIndexY, 0.f, 1.f));
	
		--m_iCurrentIcon_Index;
		m_eCurrentIcon_Index = INVENTORYICON(m_iCurrentIcon_Index);
	}

	// 현재 인벤토리에 해당하는 스킬을 넣어주어야 한다.
	if (pGameInstance->Key_Down(DIK_1))
	{
		CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEM_ONE, true);
		CUI_Manager::GetInstance()->Set_SkillIcon(ITEM_ONE, CUI_Manager::GetInstance()->Get_InventoryIcon(m_eCurrentIcon_Index));
	}
	if (pGameInstance->Key_Down(DIK_2))
	{
		CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEM_TWO, true);
		CUI_Manager::GetInstance()->Set_SkillIcon(ITEM_TWO, CUI_Manager::GetInstance()->Get_InventoryIcon(m_eCurrentIcon_Index));
	}	
	if (pGameInstance->Key_Down(DIK_3)) 
	{
		CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEM_THREE, true);
		CUI_Manager::GetInstance()->Set_SkillIcon(ITEM_THREE, CUI_Manager::GetInstance()->Get_InventoryIcon(m_eCurrentIcon_Index));
	}
	if (pGameInstance->Key_Down(DIK_4))
	{
		CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEM_FOUR, true);
		CUI_Manager::GetInstance()->Set_SkillIcon(ITEM_FOUR, CUI_Manager::GetInstance()->Get_InventoryIcon(m_eCurrentIcon_Index));
	}

	RELEASE_INSTANCE(CGameInstance);


	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, 123.667f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, 80.6667f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, 34.6667f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, -9.33333f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, -53.3333f, 0.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-138.333f, -99.3333f, 0.f, 1.f));
}

void CUI_CurrentIcon::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_CurrentIcon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_CurrentIcon::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CurrentIcon"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CurrentIcon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CUI_CurrentIcon * CUI_CurrentIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_CurrentIcon*		pInstance = new CUI_CurrentIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_CurrentIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CurrentIcon::Clone(void * pArg)
{
	CUI_CurrentIcon*		pInstance = new CUI_CurrentIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_CurrentIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CurrentIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
