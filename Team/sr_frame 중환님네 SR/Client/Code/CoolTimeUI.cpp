#include "stdafx.h"
#include "..\Header\CoolTimeUI.h"

#include "Inventory.h"
#include "Player.h"

CCoolTimeUI::CCoolTimeUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}

CCoolTimeUI::~CCoolTimeUI()
{
}

HRESULT CCoolTimeUI::Ready_Object()
{
	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);
	m_iTexNum = 4;

	switch (m_eTarget)
	{
	case CoolTimeTarget::ROLL:
		m_pIconTexture = m_pTextureCom;
		m_pIconTexture->AddRef();
		m_iIconTexNum = 5;
		break;
	case CoolTimeTarget::POTION:
		m_pIconTexture = m_pTextureCom;
		m_pIconTexture->AddRef();
		m_iIconTexNum = 17;
		break;
	case CoolTimeTarget::LEGACY1:
	case CoolTimeTarget::LEGACY2:
	case CoolTimeTarget::LEGACY3:
		m_pIconTexture = Add_Component<CTexture>(L"Proto_InventoryUI_Texture", L"Proto_InventoryUI_Texture", ID_STATIC);
		m_iIconTexNum = 0;
		break;
	default: ;
	}

	m_pCoolTimeTex = CRcTex::Create(m_pGraphicDev);
	m_iCoolTimeTexNum = 16;

	return S_OK;
}

_int CCoolTimeUI::Update_Object(const _float& fTimeDelta)
{
	CUI::Update_Object(fTimeDelta);

	if (m_bWorldSet == false)
	{
		_vec3 vScale = m_pTransCom->m_vScale;
		_vec3 vPos = m_pTransCom->m_vInfo[INFO_POS];
		_vec3 vAngle = m_pTransCom->m_vAngle;
		CGameUtilMgr::MatWorldComposeEuler(m_matCoolTimeWorld, vScale * 0.9f, vAngle, vPos);
		CGameUtilMgr::MatWorldComposeEuler(m_matIconWorld, vScale * 0.7f, vAngle, vPos);

		m_bWorldSet = true;
	}

	LEGACY_SLOT eLegacyType = LEGACY_SLOT_END;
	switch (m_eTarget)
	{
	case CoolTimeTarget::ROLL:
		{
			const CPlayer* pPlayer = Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, L"Player");
			if (pPlayer == nullptr) break;
			_float fS = pPlayer->GetRollCoolTime();
			if (fS > 1.f) fS = 1.f;
			m_pCoolTimeTex->SetProgress(1.f - fS);
			m_iIconTexNum = 5;
		}

		break;
	case CoolTimeTarget::POTION:
		{
			const CPlayer* pPlayer = Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, L"Player");
			if (pPlayer == nullptr) break;
			_float fS = pPlayer->GetPotionCoolTime();
			if (fS > 1.f) fS = 1.f;
			m_pCoolTimeTex->SetProgress(1.f - fS);
			m_iIconTexNum = 17;
		}
		break;
	case CoolTimeTarget::LEGACY1:
		eLegacyType = LEGACY_SLOT1;
		break;
	case CoolTimeTarget::LEGACY2:
		eLegacyType = LEGACY_SLOT2;
		break;
	case CoolTimeTarget::LEGACY3:
		eLegacyType = LEGACY_SLOT3;
		break;
	case CoolTimeTarget::COOL_END:
		break;
	default: ;
	}

	if (eLegacyType != LEGACY_SLOT_END)
	{
		const CInventory* pInventory = Get_GameObjectUnCheck<CPlayer>(LAYER_PLAYER, L"Player")->GetInventory();
		_float fS = pInventory->GetLegacyCoolTime(eLegacyType);
		if (fS > 1.f) fS = 1.f;
		m_pCoolTimeTex->SetProgress(1.f - fS);
		m_iIconTexNum = pInventory->GetItemUITexNum(eLegacyType);
	}

	return OBJ_NOEVENT;
}

void CCoolTimeUI::Render_Object()
{
	// old view, proj 복원은 renderer에 있음
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();

	if (m_iIconTexNum != 0)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matIconWorld);
		m_pIconTexture->Set_Texture(m_iIconTexNum);
		m_pBufferCom->Render_Buffer();
		m_iIconTexNum = 0;
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matCoolTimeWorld);
	m_pTextureCom->Set_Texture(m_iCoolTimeTexNum);

	m_pCoolTimeTex->Render_Buffer();
}

void CCoolTimeUI::Free()
{
	Safe_Release(m_pCoolTimeTex);
	CUI::Free();
}

CCoolTimeUI* CCoolTimeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum, CoolTimeTarget eTarget)
{
	CCoolTimeUI* pInstance = new CCoolTimeUI(pGraphicDev);
	pInstance->m_eTarget = eTarget;

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
