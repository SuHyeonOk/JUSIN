#include "stdafx.h"
#include "..\Header\HPUI.h"
#include "StatComponent.h"
#include "DamageFontMgr.h"

CHPUI::CHPUI(LPDIRECT3DDEVICE9 pGraphicDev): CUI(pGraphicDev)
{
}

CHPUI::~CHPUI()
{
}

HRESULT CHPUI::Ready_Object()
{
	FAILED_CHECK_RETURN(CUI::Ready_Object(), E_FAIL);
	m_iTexNum = 0;

	m_pHeartRed = CRcTex::Create(m_pGraphicDev);
	m_iHeartRedTexNum = 1;

	m_pHeartWhite = CRcTex::Create(m_pGraphicDev);
	m_iHeartWhiteTexture = 15;

	Engine::Get_Component<CStatComponent>(LAYER_PLAYER, L"Player", L"Proto_StatCom", ID_DYNAMIC)
		->m_DamageDelegater.bind(this, &CHPUI::SetProgress);

	return S_OK;
}

_int CHPUI::Update_Object(const _float& fTimeDelta)
{
	CUI::Update_Object(fTimeDelta);

	if (m_bWorldSet == false)
	{
		_vec3 vScale = m_pTransCom->m_vScale;
		_vec3 vPos = m_pTransCom->m_vInfo[INFO_POS];
		_vec3 vAngle = m_pTransCom->m_vAngle;
		vScale.x *= 0.9f;
		vScale.y *= 0.8f;
		CGameUtilMgr::MatWorldComposeEuler(m_matHeartRedWorld, vScale, vAngle, vPos);
		vScale.x *= 0.97f;
		vScale.y *= 0.97f;
		CGameUtilMgr::MatWorldComposeEuler(m_matHeartWhiteWorld, vScale, vAngle, vPos);
		m_bWorldSet = true;
	}

	if (false == CGameUtilMgr::FloatCmp(m_fFollowProgress, m_fProgress, 0.01f))
	{
		if (m_fProgress < m_fFollowProgress)
		{
			m_fFollowProgress -= fTimeDelta * 0.1f;
			m_pHeartWhite->SetProgress(m_fFollowProgress);
		}
		else
		{
			m_fFollowProgress = m_fProgress;
			m_pHeartWhite->SetProgress(m_fFollowProgress);
		}
	}


	return OBJ_NOEVENT;
}

void CHPUI::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matHeartWhiteWorld);
	m_pTextureCom->Set_Texture(m_iHeartWhiteTexture);
	m_pHeartWhite->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matHeartRedWorld);
	m_pTextureCom->Set_Texture(m_iHeartRedTexNum);
	m_pHeartRed->Render_Buffer();
}

void CHPUI::Free()
{
	Safe_Release(m_pHeartRed);
	Safe_Release(m_pHeartWhite);
	CUI::Free();
}

CHPUI* CHPUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CHPUI*		pInstance = new CHPUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CHPUI::SetProgress(_int iCurHP, _uint iMaxHP, _int iDamage)
{
	_vec2 vScreen;
	vScreen.x = m_pTransCom->m_vInfo[INFO_POS].x + WINCX / 2 - 10;
	vScreen.y = -m_pTransCom->m_vInfo[INFO_POS].y + WINCY / 2 - 60.f;

	if (iDamage < 0)
		CDamageFontMgr::GetInstance()->Add_DamageFontFromScreen(iDamage, vScreen, D3DCOLOR_ARGB(255, 255, 51, 153));
	_float fProgress = (_float)iCurHP / (_float) iMaxHP;
	m_fProgress = fProgress;
	m_pHeartRed->SetProgress(fProgress);
}
