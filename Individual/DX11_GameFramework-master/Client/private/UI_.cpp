#include "stdafx.h"
#include "..\public\UI_.h"
#include "GameInstance.h"

#include "UI_Manager.h"
#include "UI_Talk.h"

CUI_::CUI_(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_::CUI_(const CUI_ & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 0.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	CUI_*		pUI = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_UI_3DTexture */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Talk"),
		CUI_Talk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Talk")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CUI_::Tick(_double TimeDelta)
{
	if(CUI_Manager::GetInstance()->Get_Talk())
		m_vecUI[0]->Tick(TimeDelta);

	__super::Tick(TimeDelta);
}

void CUI_::Late_Tick(_double TimeDelta)
{
	if (CUI_Manager::GetInstance()->Get_Talk())
		m_vecUI[0]->Late_Tick(TimeDelta);

	__super::Late_Tick(TimeDelta);

	//if(nullptr != m_pRendererCom)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CUI_ * CUI_::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_*		pInstance = new CUI_(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_::Clone(void * pArg)
{
	CUI_*		pInstance = new CUI_(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
