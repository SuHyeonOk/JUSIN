#include "stdafx.h"
#include "..\public\UI_SkillIcon.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "UI_Manager.h"

CUI_SkillIcon::CUI_SkillIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI_(pDevice, pContext)
{
}

CUI_SkillIcon::CUI_SkillIcon(const CUI_SkillIcon & rhs)
	: CUI_(rhs)
{
}

HRESULT CUI_SkillIcon::Initialize_Prototype()
{
	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_SkillIcon::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eSkillIcon, pArg, sizeof(SKILLICON));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 87.f / 1.7f;
	m_fSizeY = 88.f / 1.7f;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.7f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));

	if(ICON_ONE == m_eSkillIcon)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-581.f, -287.f, 0.f, 1.f));
	else if (ICON_TWO == m_eSkillIcon)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-526.f, -287.f, 0.f, 1.f));
	else if (ICON_THREE == m_eSkillIcon)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-472.f, -287.f, 0.f, 1.f));
	else if (ICON_FOUR == m_eSkillIcon)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-417.f, -287.f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(g_iWinSizeX), _float(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

void CUI_SkillIcon::Tick(_double TimeDelta)
{

}

void CUI_SkillIcon::Late_Tick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_SkillIcon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_SkillIcon::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Icon"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_SkillIcon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (ICON_ONE == m_eSkillIcon)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", CUI_Manager::GetInstance()->Get_SkillIcon(0))))
			return E_FAIL;
	}
	else if (ICON_TWO == m_eSkillIcon)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", CUI_Manager::GetInstance()->Get_SkillIcon(1))))
			return E_FAIL;
	}
	else if (ICON_THREE == m_eSkillIcon)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", CUI_Manager::GetInstance()->Get_SkillIcon(2))))
			return E_FAIL;
	}
	else if (ICON_FOUR == m_eSkillIcon)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", CUI_Manager::GetInstance()->Get_SkillIcon(3))))
			return E_FAIL;
	}

	return S_OK;
}

CUI_SkillIcon * CUI_SkillIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_SkillIcon*		pInstance = new CUI_SkillIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_SkillIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_SkillIcon::Clone(void * pArg)
{
	CUI_SkillIcon*		pInstance = new CUI_SkillIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_SkillIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_SkillIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
