#include "stdafx.h"
#include "..\Header\MainApp.h"



USING(Engine)
CMainApp::CMainApp()	
	: m_pGraphicDev(nullptr)
{
}


CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp(void)
{

	// 디바이스 초기화
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd, MODE_WIN, 800, 600, &m_pDeviceClass), E_FAIL);
	m_pDeviceClass->AddRef();

	m_pGraphicDev = m_pDeviceClass->Get_GraphicDev();
	m_pGraphicDev->AddRef();

	m_pTriCol = CTriCol::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pTriCol, E_FAIL);

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
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	Engine::Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

	m_pTriCol->Render_Buffer();

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
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	Safe_Release(m_pTriCol);

	Engine::Release_Utility();
	Engine::Release_System();
}
