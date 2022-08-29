#include "stdafx.h"
#include "..\Header\MainApp.h"


CMainApp::CMainApp()	
{
}


CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp(void)
{
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

}
