#include "stdafx.h"
#include "MainGame.h"
#include "SceneMgr.h"
#include "ObjMgr.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "AstarMgr.h"
#include "BulletPoolMgr.h"

CMainGame::CMainGame()
{
}


CMainGame::~CMainGame()
{
	Release();
}

HRESULT CMainGame::Initialize(void)
{
	if (FAILED(CDevice::Get_Instance()->Initialize()))
	{
		ERR_MSG(L"Client Device Create Failed");
		return E_FAIL;
	}

	if (FAILED(CSceneMgr::Get_Instance()->Change_SceneMgr(CSceneMgr::LOADING)))
		return E_FAIL;

	CTimeMgr::Get_Instance()->Initialize();

#ifdef _DEBUG

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

#endif // _DEBUG


	return S_OK;
}

void CMainGame::Update(void)
{
	CTimeMgr::Get_Instance()->Update();
	CSceneMgr::Get_Instance()->Update_SceneMgr();
}

void CMainGame::Late_Update(void)
{
	CSceneMgr::Get_Instance()->Late_Update_SceneMgr();
}

void CMainGame::Render(void)
{
	CDevice::Get_Instance()->Render_Begin();

	CSceneMgr::Get_Instance()->Render_SceneMgr();

	CDevice::Get_Instance()->Render_End();
}

void CMainGame::Release(void)
{
#ifdef _DEBUG

	FreeConsole();

#endif // _DEBUG

	CBulletPoolMgr::Get_Instance()->Destroy_Instance();
	CAstarMgr::Get_Instance()->Destroy_Instance();
	CTimeMgr::Get_Instance()->Destroy_Instance();
	CObjMgr::Get_Instance()->Destroy_Instance();
	CTextureMgr::Get_Instance()->Destroy_Instance();
	CSceneMgr::Get_Instance()->Destroy_Instance();
	CDevice::Get_Instance()->Destroy_Instance();
}

