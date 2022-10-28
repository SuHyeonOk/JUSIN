#include "stdafx.h"
#include "..\Header\ClearUI.h"
#include "AbstFactory.h"
#include "Player.h"

CClearUI::CClearUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}


CClearUI::~CClearUI()
{
}

HRESULT CClearUI::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_UI_Texture", L"Proto_UI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_iRenderPriority = 6;

	CSoundMgr::GetInstance()->PlaySound(L"sfx_ui_missionEndVictoryCrown-001.ogg", Engine::Get_GameObject<CPlayer>(LAYER_PLAYER, L"Player")->GetInfo((INFO_POS)));
	return S_OK;
}

_int CClearUI::Update_Object(const _float & fTimeDelta)
{
	
	if (m_fCount <= 0)
	{
		CSceneFactory::LoadScene("Loading1", "Stage_Start", true, 500);
		return OBJ_DEAD;
	}		
	else
		m_wstrCount = to_wstring(_int(m_fCount -= fTimeDelta));

	CUI::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;
}

void CClearUI::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();


	Render_Font(L"Gothic_Bold70", m_wstrCount.c_str(),
		&_vec2(WINCX * 0.48f, WINCY *0.19f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	Render_Font(L"Gothic_Bold20", L"임무 종료중... ",
		&_vec2(WINCX * 0.44f, WINCY *0.35f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CClearUI::Free()
{
	CUI::Free();
}

CClearUI * CClearUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CClearUI* pInstance = new CClearUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
