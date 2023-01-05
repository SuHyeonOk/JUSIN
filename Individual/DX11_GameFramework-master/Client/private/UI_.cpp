#include "stdafx.h"
#include "..\public\UI_.h"
#include "GameInstance.h"

#include "UI_Manager.h"

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

	// [0] : Npc 와의 대화창
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Talk")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [1] : Monster 공격시 UI
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Monster")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [2] : Monster 공격시 UI
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Monster_HealthBar_Bar")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [3] : Monster 공격시 UI
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Monster_HealthBar_BarBack")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [4] : Player Indormacion
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_Informacion")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [5] : Player HpBar
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_HPBar")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [6] : Player HpBarBack
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Player_HPBarBack")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [7] : Player LevelBar
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Level_HPBar")));

	if (nullptr == pUI)
		return E_FAIL;

	m_vecUI.push_back(pUI);

	// [8] : Player LevelBarBack
	pUI = dynamic_cast<CUI_*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Level_HPBarBack")));

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

	if (CUI_Manager::GetInstance()->Get_UI_Monster())
	{
		m_vecUI[1]->Tick(TimeDelta);
		m_vecUI[3]->Tick(TimeDelta);
		m_vecUI[2]->Tick(TimeDelta);
	}

	m_vecUI[4]->Tick(TimeDelta);
	m_vecUI[6]->Tick(TimeDelta);
	m_vecUI[5]->Tick(TimeDelta);
	m_vecUI[8]->Tick(TimeDelta);
	m_vecUI[7]->Tick(TimeDelta);

	__super::Tick(TimeDelta);
}

void CUI_::Late_Tick(_double TimeDelta)
{
	if (CUI_Manager::GetInstance()->Get_Talk())
		m_vecUI[0]->Late_Tick(TimeDelta);

	if (CUI_Manager::GetInstance()->Get_UI_Monster())
	{
		m_vecUI[1]->Late_Tick(TimeDelta);
		m_vecUI[3]->Late_Tick(TimeDelta);
		m_vecUI[2]->Late_Tick(TimeDelta);
	}

	m_vecUI[4]->Late_Tick(TimeDelta);
	m_vecUI[6]->Late_Tick(TimeDelta);
	m_vecUI[5]->Late_Tick(TimeDelta);
	m_vecUI[8]->Late_Tick(TimeDelta);
	m_vecUI[7]->Late_Tick(TimeDelta);

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
