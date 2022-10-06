#include "stdafx.h"
#include "..\Header\TalkWindow.h"

#include "Export_Function.h"

CTalkWindow::CTalkWindow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_iTextCount(0)
{
}

CTalkWindow::~CTalkWindow()
{
}

HRESULT CTalkWindow::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matWorld);

	m_fScaleX = 300.f;
	m_fScaleY = 120.f;

	m_fPosX = 0.f;
	m_fPosY = 0.f;

	// 스케일 값
	D3DXMatrixScaling(&m_matView, m_fScaleX, m_fScaleY, 1.f);

	// 포지션
	m_matView._41 = m_fTempPosX;
	m_matView._42 = m_fTempPosY;

	return S_OK;
}

_int CTalkWindow::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	if (!m_bText)
		return 0;
	else
	{
		m_matView._41 = m_fPosX;
		m_matView._42 = m_fPosY;
	}

	return 0;
}

void CTalkWindow::LateUpdate_Object(void)
{
	if (!m_bText)
		return;

	Engine::CGameObject::LateUpdate_Object();
}

void CTalkWindow::Render_Obejct(void)
{
	if (!m_bText)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(0);	// 텍스처 정보 세팅을 우선적으로 한다.
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	if (m_iTextCount < m_vecDialogue.size())
		Render_Font(L"Font_Jinji", m_vecDialogue[m_iTextCount].c_str(), &_vec2(270, 250), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	else
	{
		m_bText = false;
		m_iTextCount = 0;
		m_vecDialogue.clear();
	}
}

HRESULT CTalkWindow::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// m_pTextureCom	
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TalkWindow_Texture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TalkWindow_Texture", pComponent });
	
	return S_OK;
}

CTalkWindow * CTalkWindow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTalkWindow *	pInstance = new CTalkWindow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTalkWindow::Free(void)
{
	CUI::Free();
}
