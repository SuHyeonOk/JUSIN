#include "stdafx.h"
#include "Device.h"

IMPLEMENT_SINGLETON(CDevice)

CDevice::CDevice()
	: m_pSDK(nullptr), m_pDevice(nullptr), m_pSprite(nullptr), m_pFont(nullptr)
{
}


CDevice::~CDevice()
{
	Release();
}

HRESULT CDevice::Initialize(void)
{
	// 장치 초기화 과정

	// 1. 렌더링 장치를 생성하고 조사할 객체 생성

	// Direct3DCreate9 : SDK 버전에 맞게 생성하는 함수,D3D_SDK_VERSION 버전은 우리가 포함한 DX헤더에 정의되어 있ㅇㅁ
	// m_pSDK객체는 장치 검증과 LPDIRECT3DDEVICE9 객체 생성 두 가지 용도로 이용
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
		
	// 2. 장치 조사(지원 수준을 파악)

	// HAL : 하드웨어 추상화 계층, 특정 플랫폼에서 동작할 하드웨어에 대한 데이터 정보들이 필요한데 하드웨어 제조사는 다양하다.
	// 제조사가 다를지라도 특정 플랫폼에서 동일하게 동작하기 위해 데이터 정보를 추상화시켜 구현을 해둔다.
	
	// HAL을 통하여 현재 장치에 대한 정보를 얻은 뒤 D3DCAPS9 구조체에 기록
	D3DCAPS9			DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// SUCCEEDED() : 값이 양수일 때 TRUE 리턴
	// FAILED : 값이 음수일 때 TRUE 리턴

	// GetDeviceCaps : 장치에 대한 지원 수준을 얻어오는 함수
	// D3DADAPTER_DEFAULT : 정보를 얻으려는 기본 그래픽 카드를 의미

	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		ERR_MSG(L"Get Device Caps Failed");
		return E_FAIL;
	}

	DWORD		vp = 0;	

	// HAL을 통해 조사한 장치게 하드웨어 버텍스 프로세싱을 지원한다면
	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;	// 하드웨어가 연산

	else	// 그렇지 않다면 사용자가 직접 연산
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	// 3. 렌더링 장치 제어 객체 생성

	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));


	Set_Parameters(d3dpp);

	// CreateDevice : 그래픽 장치를 제어할 객체를 생성하는 함수

	// 1. 어떤 그래픽 카드를 제어할 것인가
	// 2. 어떤 정보로 장치에 접근할 것인가
	// 3. 장치를 사용할 윈도우 핸들
	// 4. 동작 방식
	// 5. 사용 환경
	// 6. 생성할 객체의 주소값을 저장하기 위한 포인터

	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL, 
									g_hWnd, 
									vp, 
									&d3dpp, 
									&m_pDevice)))
	{
		ERR_MSG(L"CreateDevice Failed");
		return E_FAIL;	}

	// 스프라이트 객체 생성

	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		ERR_MSG(L"CreateSprite Failed");
		return E_FAIL;
	}

	// 폰트 객체 생성

	D3DXFONT_DESCW			tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));

	tFontInfo.Height = 20;
	tFontInfo.Width  = 10;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"궁서");

	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont)))
	{
		ERR_MSG(L"Create Font Failed");
		return E_FAIL;
	}
	
	return S_OK;
}

void CDevice::Release(void)
{
	if (nullptr != m_pFont)
		m_pFont->Release();

	if (nullptr != m_pSprite)
		m_pSprite->Release();

	if (nullptr != m_pDevice)
		m_pDevice->Release();

	if (nullptr != m_pSDK)
		m_pSDK->Release();
}

void CDevice::Render_Begin(void)
{
	// 렌더링 과정 : 후면 버퍼를 비운다 -> 후면 버퍼에 그린다 -> 후면 버퍼와 전면 버퍼를 교체 -> 화면 출력

	/*RECT	rc[10] { }*/

	m_pDevice->Clear(0,			// 렉트의 개수
					nullptr,	// 렉트 배열의 첫 번째 주소
					D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // 비우고자하는 버퍼
					D3DCOLOR_ARGB(255, 125, 125, 125), // 후면버퍼 색상
					1.f, // z버퍼 초기화 값
					0);  // 스텐실 버퍼 초기화 값
		
	// 여기서부터 후면 버퍼에 그린다.
	m_pDevice->BeginScene();

	// 2D 이미지를 그릴 수 있도록 장치에게 알림(현재 렌더링 옵션)
	// 현재 렌더링 옵션은 알파 테스트가 유효한 상태에서 알파 블렌딩을 사용하겠다는 옵션
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

}


void CDevice::Render_End(HWND hWnd)
{

	m_pSprite->End();

	// 여기까지가 후면 버퍼에 다 그린 시점
	m_pDevice->EndScene();

	// 후면 버퍼와 전면 버퍼를 교환하여 화면에 출력

	// 1,2번 인자 : 스왑체인 옵션이 D3DSWAPEFFECT_COPY로 작성하지 않은 한 NULL
	// 3번 인자 : 출력 대상 윈도우 핸들, NULL인 경우 Set_Parameters에서 지정한 핸들이 기본 값으로 설정됨
	// 4번 인자 : 스왑체인 옵션이 D3DSWAPEFFECT_COPY로 작성하지 않은 한 NULL

	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);

}

void CDevice::Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp)
{
	d3dpp.BackBufferWidth = WINCX;
	d3dpp.BackBufferHeight = WINCY;

	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
/*
	D3DSWAPEFFECT_DISCARD : 스왑 체인
	D3DSWAPEFFECT_FLIP : 버퍼 하나로 뒤집으면서 사용하는 방식
	D3DSWAPEFFECT_COPY : 더블 버퍼링과 유사한 복사 방식
*/
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// 장치를 사용할 윈도우 핸들 지정
	d3dpp.hDeviceWindow = g_hWnd;

	// 창 모드로 실행할 것인지 여부를 물음
	d3dpp.Windowed = TRUE;		// true 인 경우 창 모드
	
	// 그리기 버퍼 종류 : 타겟, 스텐실, Z(깊이)

	// DX가 자동적으로 깊이 / 스텐실 버퍼를 만들고 관리하기 원하는가
	d3dpp.EnableAutoDepthStencil = TRUE;

	// 깊이 버퍼와 스테실 버퍼의 포맷 설정
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// 화면이 전체화면 시에만 사용되는 매개변수들
	// 창 모드에서는 OS가 모니터 재생률을 관리

	// 전체 화면 시에는 그래픽 카드가 현재 모니터 재생률을 토대로 수행
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	// 모니터 재생률과 시연(프로그램 프레임)의 간격 설정

	// D3DPRESENT_INTERVAL_IMMEDIATE : 즉시 시연
	// D3DPRESENT_INTERVAL_DEFAULT : 적절한 간격을 DX가 알아서 결정, 보통 모니터 재생률을 따라감

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}
