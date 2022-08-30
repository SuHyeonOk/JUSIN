#include "stdafx.h"
#include "..\Header\MainApp.h"


USING(Engine)
CMainApp::CMainApp()	
{
}


CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp(void)
{
	// µð¹ÙÀÌ½º ÃÊ±âÈ­
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd, MODE_WIN, 800, 600, &m_pDeviceClass), E_FAIL);

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float & fTimeDelta)
{
	return 0;
}

void CMainApp::LateUpdate_MainApp(void)
{
}

void CMainApp::Render_MainApp(void)
{
	Engine::Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

	Engine::Render_End();
}

CMainApp * CMainApp::Create(void)
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("MainApp Create Failed");
		Safe_Release<CMainApp*>(pInstance);		
	}
	
	return pInstance;
}

void CMainApp::Free(void)
{
	Engine::Release_System(); // ½Ì±ÛÅæ ÆÄ±¥
}
