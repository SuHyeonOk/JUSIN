#include "stdafx.h"
#include "..\Header\PlayerUI.h"


CPlayerUI::CPlayerUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}


CPlayerUI::~CPlayerUI()
{
}

HRESULT CPlayerUI::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_UI_Texture", L"Proto_UI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_iRenderPriority = 6;
	return S_OK;
}

_int CPlayerUI::Update_Object(const _float & fTimeDelta)
{
	if (m_bClose)
		return OBJ_DEAD;

	if (m_fCount == 0)
		m_fCount = 0;
	else
		m_wstrCount = to_wstring(_int(m_fCount -= fTimeDelta));

	CUI::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;
}

void CPlayerUI::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();

	Render_Font(L"Gothic_Bold30", L"사망하셨습니다",
		&_vec2(WINCX * 0.42f, WINCY *0.41f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	if (m_bCount)
	{
		wstring wtrCount = L"시작까지 " + m_wstrCount + L"초";
		Render_Font(L"Gothic_Bold20", wtrCount.c_str(),
			&_vec2(WINCX * 0.42f, WINCY *0.5f),
			D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}

}

void CPlayerUI::Free()
{
	CUI::Free();
}

CPlayerUI * CPlayerUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CPlayerUI* pInstance = new CPlayerUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

