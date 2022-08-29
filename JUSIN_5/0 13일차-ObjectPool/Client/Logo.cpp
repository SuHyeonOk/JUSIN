#include "stdafx.h"
#include "Logo.h"
#include "TextureMgr.h"
#include "Device.h"
#include "SceneMgr.h"


CLogo::CLogo()
{
}


CLogo::~CLogo()
{
	Release_Scene();
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(TEX_SINGLE, L"../Texture/JusinLogo.png", L"Logo")))
		return E_FAIL;

	
	InitializeCriticalSection(&m_Crt);

	int		iNumber = 10;

	m_hThread = (HANDLE)_beginthreadex(nullptr,		// 디폴트 보안 속성(핸들의 상속 여부, nullptr인 경우 상속에서 제외)
										0,			// 디폴트 스택 사이즈(1바이트)
										ImgLoadThreadFunc,  // 쓰레드 함수(__stdcall 규약)
										this,  // 쓰레드 함수 전달 인자
										0,		// 쓰레드의 생성 및 실행을 조절하기 위한 FLAG값
										nullptr); // 쓰레드 ID반환


	return S_OK;
}

void CLogo::Update_Scene()
{
	if (GetAsyncKeyState(VK_RETURN))
	{
		// 쓰레드가 종료될 때까지 대기
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);

		DeleteCriticalSection(&m_Crt);
		CSceneMgr::Get_Instance()->Change_SceneMgr(CSceneMgr::STAGE);
		return;
	}
	
}

void CLogo::Late_Update_Scene()
{
	
}

void CLogo::Render_Scene()
{
	const TEXINFO*		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Logo");

	float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
	float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

	D3DXMATRIX	matTrans;
	D3DXMatrixTranslation(&matTrans, WINCX / 2.f, WINCY / 2.f, 0.f);
	
	CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matTrans);

	CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,
		nullptr,
		&D3DXVECTOR3(fCenterX, fCenterY, 0.f),
		nullptr,
		D3DCOLOR_ARGB(255, 255, 255, 255));

	wstring				wstrFullPath = CTextureMgr::Get_Instance()->Get_String();

	D3DXMatrixTranslation(&matTrans, 50.f, 500.f, 0.f);
	CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matTrans);
	CDevice::Get_Instance()->Get_Font()->DrawTextW(CDevice::Get_Instance()->Get_Sprite(),
		wstrFullPath.c_str(), 
		wstrFullPath.length(), 
		nullptr, 0,
		D3DCOLOR_ARGB(255, 0, 0, 0));
}

void CLogo::Release_Scene()
{
	
}

size_t __stdcall CLogo::ImgLoadThreadFunc(void* pVoid)
{
	CLogo*	pLogo = reinterpret_cast<CLogo*>(pVoid);


	EnterCriticalSection(&pLogo->Get_Crt());

	if (FAILED(CTextureMgr::Get_Instance()->Read_ImgPath(L"../Data/ImgPath.txt")))
		return E_FAIL;

	CTextureMgr::Get_Instance()->Set_String(L"로딩 완료");

	LeaveCriticalSection(&pLogo->Get_Crt());

	// _endthreadex(0);

	return 0;
}

CLogo* CLogo::Create(void)
{
	CLogo*	pScene = new CLogo;

	if (FAILED(pScene->Ready_Scene()))
		return nullptr;

	return pScene;
}
