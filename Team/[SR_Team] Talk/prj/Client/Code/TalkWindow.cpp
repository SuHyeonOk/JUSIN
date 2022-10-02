#include "stdafx.h"
#include "..\Header\TalkWindow.h"

#include "Export_Function.h"

CTalkWindow::CTalkWindow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_iTextCount(0)
{
	ZeroMemory(m_szNPCText, sizeof(m_szNPCText));
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
	{
		swprintf_s(m_szNPCText, L"");
		
		m_matView._41 = m_fTempPosX;
		m_matView._42 = m_fTempPosY;

		return 0;
	}

	if (Engine::Get_DIKeyState(DIK_T) & 0X80) // 한 번만 눌리도록 해야함
	{
		TextSetting();
		m_iTextCount++;
		
		m_matView._41 = m_fPosX;
		m_matView._42 = m_fPosY;
	}
	if (Engine::Get_DIKeyState(DIK_RETURN) & 0X80)
	{
		m_bText = false;
		swprintf_s(m_szNPCText, L"");

		m_matView._41 = m_fTempPosX;
		m_matView._42 = m_fTempPosY;
	}
	return 0;
}

void CTalkWindow::LateUpdate_Object(void)
{
	Engine::CGameObject::LateUpdate_Object();
}

void CTalkWindow::Render_Obejct(void)
{
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

	Render_Font(L"Font_Jinji", m_szNPCText, &_vec2(270, 250), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
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

void CTalkWindow::TextSetting()
{
	if (!m_bText)
		return;

	switch (m_iTextCount)
	{
	case 0:
		swprintf_s(m_szNPCText, L"멍..");
		break;
	case 1:
		swprintf_s(m_szNPCText, L"멍..멍..");
		break;
	case 3:
		swprintf_s(m_szNPCText, L"멍..멍..멍..");
		break;
	case 4:
		swprintf_s(m_szNPCText, L"왈!");
		break;
	default:
		swprintf_s(m_szNPCText, L"...");
		break;
	}	
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
