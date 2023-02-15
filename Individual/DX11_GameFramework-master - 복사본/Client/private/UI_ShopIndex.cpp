#include "stdafx.h"
#include "..\public\UI_ShopIndex.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_ShopIndex::CUI_ShopIndex(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_ShopIndex::CUI_ShopIndex(const CUI_ShopIndex & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_ShopIndex::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ShopIndex::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 321.0f / 1.5f;
	m_fSizeY = 80.0f / 1.5f;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.7f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-96.0f, 33.0f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	m_arr_ShopIndx[_int(SHOPINDEX::SHOP_ONE)] = CSkill_Manager::PLAYERSKILL::SKILL::PAINT;
	m_arr_ShopIndx[_int(SHOPINDEX::SHOP_TWO)] = CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT;
	m_arr_ShopIndx[_int(SHOPINDEX::SHOP_THREE)] = CSkill_Manager::PLAYERSKILL::SKILL::COIN;
	m_arr_ShopIndx[_int(SHOPINDEX::SHOP_FOUR)] = CSkill_Manager::PLAYERSKILL::SKILL::FIONA;
	m_arr_ShopIndx[_int(SHOPINDEX::SHOP_FIVE)] = CSkill_Manager::PLAYERSKILL::SKILL::FOOD;
	m_arr_ShopIndx[_int(SHOPINDEX::SHOP_SIX)] = CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END;	// Ä®

	return S_OK;
}

void CUI_ShopIndex::Tick(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_RIGHT) && 5 > m_iShopIndex)	// ¿À¸¥ÂÊ
	{
		pGameInstance->Play_Sound(TEXT("sfx_click.ogg"), 0.7f);

		m_iIndexTexture = 1; // ÆÄ¶õ»ö
		++m_iShopIndex;
		m_eShopIndex = SHOPINDEX(m_iShopIndex);
	}

	if (pGameInstance->Key_Down(DIK_LEFT) && 0 < m_iShopIndex)	// ¿ÞÂÊ								
	{
		pGameInstance->Play_Sound(TEXT("sfx_click.ogg"), 0.7f);
		
		m_iIndexTexture = 1; // ÆÄ¶õ»ö
		--m_iShopIndex;
		m_eShopIndex = SHOPINDEX(m_iShopIndex);
	}

	if (pGameInstance->Key_Down(DIK_SPACE))						// °è»ê								
	{
		if (false == Calculation())
		{
			if(CObj_Manager::PLAYERINFO::PLAYER::FINN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
				pGameInstance->Play_Sound(TEXT("v_Finn_NoMoney.ogg"), 1.0f);
			else
				pGameInstance->Play_Sound(TEXT("v_Jake_NoMoney.ogg"), 1.0f);

			m_iIndexTexture = 2;	// »ßºò »¡°£»ö
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		else
			pGameInstance->Play_Sound(TEXT("sfx_click2.ogg"), 0.7f);

		if (5 == m_iShopIndex)
		{
			pGameInstance->Play_Sound(TEXT("sfx_enchiridion_level_up.ogg"), 0.7f);

			CObj_Manager::GetInstance()->Set_Finn_Sword(CObj_Manager::PLAYERINFO::PLAYERWEAPON::F_ROOT);
			
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		CSkill_Manager::PLAYERSKILL::SKILL	eSkill = m_arr_ShopIndx[m_iShopIndex];
		CSkill_Manager::GetInstance()->Page_PickUp(eSkill);
	}

	RELEASE_INSTANCE(CGameInstance);

	IndexPosition();
}

void CUI_ShopIndex::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_ShopIndex::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_ShopIndex::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Shop"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ShopIndex::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndexTexture)))
		return E_FAIL;

	return S_OK;
}

void CUI_ShopIndex::IndexPosition()
{
	switch (m_eShopIndex)
	{
	case Client::SHOPINDEX::SHOP_ONE:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-97.0f, 33.3333f, 0.f, 1.f));
		break;
	case Client::SHOPINDEX::SHOP_TWO:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(117.0f, 33.3333f, 0.f, 1.f));
		break;
	case Client::SHOPINDEX::SHOP_THREE:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-97.0f, -22.6667f, 0.f, 1.f));
		break;
	case Client::SHOPINDEX::SHOP_FOUR:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(117.0f, -22.6667f, 0.f, 1.f));
		break;
	case Client::SHOPINDEX::SHOP_FIVE:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-97.0f, -78.6667f, 0.f, 1.f));
		break;
	case Client::SHOPINDEX::SHOP_SIX:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(117.0f, -78.6667f, 0.f, 1.f));
		break;
	}
}

_bool CUI_ShopIndex::Calculation()
{
	switch (m_eShopIndex)
	{
	case Client::SHOPINDEX::SHOP_ONE:	// "PAINT"
	{
		if (100 <= CObj_Manager::GetInstance()->Get_Current_Player().iCoin)
		{
			CObj_Manager::GetInstance()->Set_Coin(-100);
			return true;
		}
	}
		break;
	case Client::SHOPINDEX::SHOP_TWO:	// "MARCELINE"
	{
		if (130 <= CObj_Manager::GetInstance()->Get_Current_Player().iCoin)
		{
			CObj_Manager::GetInstance()->Set_Coin(-130);
			return true;
		}
	}
		break;
	case Client::SHOPINDEX::SHOP_THREE:	// "GOLD"
	{
		if (50 <= CObj_Manager::GetInstance()->Get_Current_Player().iCoin)
		{
			CObj_Manager::GetInstance()->Set_Coin(-50);
			return true;
		}
	}
		break;
	case Client::SHOPINDEX::SHOP_FOUR:	// "FIONNA"
	{
		if (150 <= CObj_Manager::GetInstance()->Get_Current_Player().iCoin)
		{
			CObj_Manager::GetInstance()->Set_Coin(-150);
			return true;
		}
	}
		break;
	case Client::SHOPINDEX::SHOP_FIVE:	// "FOOD"
	{
		if (70 <= CObj_Manager::GetInstance()->Get_Current_Player().iCoin)
		{
			CObj_Manager::GetInstance()->Set_Coin(-70);
			return true;
		}
	}
		break;
	case Client::SHOPINDEX::SHOP_SIX:	// "SOWRD"
	{
		_int iValue;
		if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
			iValue = 300;
		else
			iValue = 9999999;

		if (iValue <= CObj_Manager::GetInstance()->Get_Current_Player().iCoin)
		{
			CObj_Manager::GetInstance()->Set_Coin(-iValue);
			return true;
		}
	}
		break;
	}

	return false;
}

CUI_ShopIndex * CUI_ShopIndex::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_ShopIndex*		pInstance = new CUI_ShopIndex(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_ShopIndex");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_ShopIndex::Clone(void * pArg)
{
	CUI_ShopIndex*		pInstance = new CUI_ShopIndex(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_ShopIndex");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ShopIndex::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
