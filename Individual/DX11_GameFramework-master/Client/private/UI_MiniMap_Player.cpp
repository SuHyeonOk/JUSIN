#include "stdafx.h"
#include "..\public\UI_MiniMap_Player.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_MninMap_Player::CUI_MninMap_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_MninMap_Player::CUI_MninMap_Player(const CUI_MninMap_Player & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_MninMap_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_MninMap_Player::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 14.f / 3.5f;
	m_fSizeY = 14.f / 3.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
	{
		m_fPlayer_X = 547.f;
		m_fPlayer_Y = 223.f;
	}

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fPlayer_X, m_fPlayer_Y, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_MninMap_Player::Tick(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_fPlayer_X -= _float(TimeDelta * 3.5f);
	if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_fPlayer_X += _float(TimeDelta * 3.5f);
	if (pGameInstance->Key_Pressing(DIK_UP))
		m_fPlayer_Y += _float(TimeDelta * 3.5f);
	if (pGameInstance->Key_Pressing(DIK_DOWN))
		m_fPlayer_Y -= _float(TimeDelta * 3.5f);

	RELEASE_INSTANCE(CGameInstance);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fPlayer_X, m_fPlayer_Y, 0.f, 1.f));

	RELEASE_INSTANCE(CGameInstance);
}

void CUI_MninMap_Player::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_MninMap_Player::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar szCount[MAX_PATH];
	wsprintf(szCount, TEXT("Jake Son : %d"), CUI_Manager::GetInstance()->Get_JakeSon_Count());

	pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(1137.f, 164.f), 0.f, _float2(0.35f, 0.33f));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_MninMap_Player::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mini_Map_Player"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MninMap_Player::SetUp_ShaderResources()
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

CUI_MninMap_Player * CUI_MninMap_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_MninMap_Player*		pInstance = new CUI_MninMap_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MninMap_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MninMap_Player::Clone(void * pArg)
{
	CUI_MninMap_Player*		pInstance = new CUI_MninMap_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MninMap_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MninMap_Player::Free()
{
	__super::Free();
}
