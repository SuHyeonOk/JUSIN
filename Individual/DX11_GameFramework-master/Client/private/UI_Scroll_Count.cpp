#include "stdafx.h"
#include "..\public\UI_Scroll_Count.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Skill_Manager.h"

CUI_Scroll_Count::CUI_Scroll_Count(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_Scroll_Count::CUI_Scroll_Count(const CUI_Scroll_Count & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_Scroll_Count::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Scroll_Count::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eItemCount, pArg, sizeof(ITEMINDEX));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 32.f / 1.7f;
	m_fSizeY = 31.f / 1.7f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if (ITEM_ONE == m_eItemCount)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-598.f, -308.f, 0.f, 1.f));
	else if (ITEM_TWO == m_eItemCount)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-542.f, -308.f, 0.f, 1.f));
	else if (ITEM_THREE == m_eItemCount)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-488.f, -308.f, 0.f, 1.f));
	else if (ITEM_FOUR == m_eItemCount)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-433.f, -308.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_Scroll_Count::Tick(_double TimeDelta)
{

}

void CUI_Scroll_Count::Late_Tick(_double TimeDelta)
{

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Scroll_Count::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CSkill_Manager::PLAYERSKILL tPlayerSkill;
	if (ITEM_ONE == m_eItemCount)
	{
		tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE);
		_int iCount = CSkill_Manager::GetInstance()->Get_PageCount(tPlayerSkill.eSkill);
		_tchar szCount[5];
		_itow_s(iCount, szCount, sizeof(szCount));

		pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(34.f, 655.f), 0.f, _float2(0.3f, 0.25f));
	}
	else if (ITEM_TWO == m_eItemCount)
	{
		tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_TWO);
		_int iCount = CSkill_Manager::GetInstance()->Get_PageCount(tPlayerSkill.eSkill);
		_tchar szCount[5];
		_itow_s(iCount, szCount, sizeof(szCount));

		pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(90.f, 655.f), 0.f, _float2(0.3f, 0.25f));
	}
	else if (ITEM_THREE == m_eItemCount)
	{
		tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_THREE);
		_int iCount = CSkill_Manager::GetInstance()->Get_PageCount(tPlayerSkill.eSkill);
		_tchar szCount[5];
		_itow_s(iCount, szCount, sizeof(szCount));

		pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(144.f, 655.f), 0.f, _float2(0.3f, 0.25f));
	}
	else if (ITEM_FOUR == m_eItemCount)
	{
		tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_FOUR);
		_int iCount = CSkill_Manager::GetInstance()->Get_PageCount(tPlayerSkill.eSkill);
		_tchar szCount[5];
		_itow_s(iCount, szCount, sizeof(szCount));

		pGameInstance->Render_Font(TEXT("Font_Comic"), szCount, _float2(199.f, 655.f), 0.f, _float2(0.3f, 0.25f));
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Scroll_Count::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Scroll_Count"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Scroll_Count::SetUp_ShaderResources()
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

CUI_Scroll_Count * CUI_Scroll_Count::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_Scroll_Count*		pInstance = new CUI_Scroll_Count(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Scroll_Count");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Scroll_Count::Clone(void * pArg)
{
	CUI_Scroll_Count*		pInstance = new CUI_Scroll_Count(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Scroll_Count");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Scroll_Count::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
