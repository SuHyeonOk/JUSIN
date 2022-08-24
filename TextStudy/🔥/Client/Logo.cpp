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

	
	InitializeCriticalSection(&m_Crt); // 쓰레드 동기화를 위해서 쓰는 동기화 모델(쓰레드 생성은 별도)

	int		iNumber = 10;

	// _beginthreadex : 쓰레드 생성 함수
	m_hThread = (HANDLE)_beginthreadex(nullptr,		// 디폴트 보안 속성(핸들의 상속 여부, nullptr인 경우 상속에서 제외)
		// 개발한 쓰레드를 다른 프로세스에서 사용하고자 할 때 이 쓰레드를 허가 할 것인가, 말 것 인가
		// 당연히 허가하지 않는다! nullptr을 넣어서 알아서 보안을 하도록 한다
										0,			// 디폴트 스택 사이즈(1바이트)
		// 0을 입력하면 1byte가 할당되는데 필요하면 스텍 메모리에 있는 용량을 최대 가용하겠다는 것 이다
										ImgLoadThreadFunc,  // 쓰레드 함수(__stdcall 규약)
										this,		// 쓰레드 함수 전달 인자
		//this의 경우 static size_t __stdcall ImgLoadThreadFunc(void* pVoid); 함수의 매개변수를 넣는 곳 이다
		//현재의 경우 쓰레드를 통해 로코 클래스를 조작하고 싶기에 this를 넣어주었지만
		//예시로 int iNumber = 10; 이 있을 때 iNumber를 쓰레드를 통해서 값을 바꾸고 싶다면 this위치에 &void를 넣어주면 된다
		//void포인터 형으로 int형 주소가 들어가게 되어 함수 안 에서 코드로 값을 바꿀 수 있도록 한다
										0,			// 쓰레드의 생성 및 실행을 조절하기 위한 FLAG값 (옵션)
										nullptr);	// ★기억하기★쓰레드 ID반환
	// 현재는 쓰레드를 하나만 만들기 때문에 defate로 무조건 0번 쓰레드 ID가 반환되지만 
	// 멀티 쓰레드의 경우 이 핸들이 배열이여야 한다 (여러개의 쓰레드 마다 각자마다의 핸들이 있어야 하기 때문에)
	// DWORD dwID[i] 라고 개수만큼 배열을 잡아두고 이 주소를 넣어주게 되면
	// 생성된 쓰레드 마다 고유의 ID를 이 nullptr 인자(쓰레드 ID반환)에 있는 애가 가지고 있다 
	// 현재는 멀티 쓰레드가 아니기 때문에 위의 위의 방식처럼 작성하지 않았다
	// 쉽게 이야기 해서 파일을 개방한 것 처럼 쓰레드도 개방을 했다는 것 이다
	// 프로세스 핸들을 만드는 것 처럼 쓰레드 핸들을 만들어서 넣어주었다고 생각하면 된다 

	return S_OK;
}

void CLogo::Update_Scene()
{
	if (GetAsyncKeyState(VK_RETURN))
	{
		// ﻿현재 구동되는 쓰레드(INFINITE)가 종료될 때 까지 대기 한다
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread); // 생성했던 쓰레드 핸들 지우기

		DeleteCriticalSection(&m_Crt); // 생성했었던 CRITICAL_SECTION m_Crt을 지워준다
		CSceneMgr::Get_Instance()->Change_SceneMgr(CSceneMgr::STAGE); // 그 다음 씬을 변경한다
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
	CLogo*	pLogo = reinterpret_cast<CLogo*>(pVoid); // 매개변수로 들어오는 pVoid을 CLogo*로 형변환

	if (FAILED(CTextureMgr::Get_Instance()->Read_ImgPath(L"../Data/ImgPath.txt")))
		return E_FAIL; // ﻿스테이지에서 사용할 이미지를 여기서 추가

	EnterCriticalSection(&pLogo->Get_Crt()); // ﻿크리티컬 섹션(Critical Section)을 이용한 쓰레드 동기화 기법을 이용﻿(메모리를 점유하는 방식)
											 // Create변수를 static함수 ImgLoadThreadFunc에서 사용하기 위해 Get_Crt()을 만들어 사용

	CTextureMgr::Get_Instance()->Set_String(L"로딩 완료"); // 로딩이 다 끝났을 때 눈으로 알려줄 수 있도록

	LeaveCriticalSection(&pLogo->Get_Crt()); // 나중에 쓰레드를 다 돌고나서 더 이상 필요 없으면 임계 영역을 벗어나겠다

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
