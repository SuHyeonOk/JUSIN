#include "..\..\Header\GraphicDev.h"

USING(Engine)
IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
	:m_pGraphicDev(nullptr)
	, m_pSDK(nullptr)
{
}


CGraphicDev::~CGraphicDev()
{
	Free();
}

HRESULT CGraphicDev::Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint & iSizeX, const _uint & iSizeY, CGraphicDev ** ppGraphicDev)
{
	// 장치 초기화 과정
	// 그리기를 통제할 객체를 생성하기 위한 원본이 필요하다.

	// 1. 렌더링 장치를 생성하고 조사할 객체 생성

	// Direct3DCreate9 : SDK 버전에 맞게 생성하는 함수,D3D_SDK_VERSION 버전은 우리가 포함한 DX헤더에 정의되어 있음
	// m_pSDK객체는 장치 검증과 LPDIRECT3DDEVICE9 객체 생성 두 가지 용도로 이용
	// SDK : 그래픽 카드를 통제하기 위한 원본 객체
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	NULL_CHECK_RETURN(m_pSDK, E_FAIL);

	// 2. 장치 조사(지원 수준을 파악)

	// HAL : 하드웨어 추상화 계층, 특정 플랫폼에서 동작할 하드웨어에 대한 데이터 정보들이 필요한데 하드웨어 제조사는 다양하다.
	// 제조사가 다를지라도 특정 플랫폼에서 동일하게 동작하기 위해 데이터 정보를 추상화시켜 구현을 해둔다.

	// HAL을 통하여 현재 장치에 대한 정보를 얻은 뒤 D3DCAPS9 구조체에 기록
	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// GetDeviceCaps : 장치에 대한 지원 수준을 얻어오는 함수
	// D3DADAPTER_DEFAULT : 정보를 얻으려는 기본 그래픽 카드를 의미
	FAILED_CHECK_RETURN(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps), E_FAIL);

	_ulong	dwFlag = 0;

	// HAL을 통해 조사한 장치가 하드웨어 버텍스 프로세싱을 지원한다면
	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; // 하드웨어가 연산

	else	// 그렇지 않다면 사용자가 직접 연산
		dwFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	// 3. 렌더링 장치 제어 객체 생성
	D3DPRESENT_PARAMETERS		Present_Parameters;
	ZeroMemory(&Present_Parameters, sizeof(D3DPRESENT_PARAMETERS));

	Present_Parameters.BackBufferWidth = iSizeX;
	Present_Parameters.BackBufferHeight = iSizeY;
	Present_Parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	Present_Parameters.BackBufferCount = 1;

	Present_Parameters.MultiSampleType = D3DMULTISAMPLE_NONE; // MultiSampleType  : 안티애일리어싱(계단현상방지)=>속도저하 유발.
	Present_Parameters.MultiSampleQuality = 0;

	Present_Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;// 스왑체인 방식을 채택
	Present_Parameters.hDeviceWindow = hWnd;

	Present_Parameters.Windowed = eMode; // 창모드/전체모드

	Present_Parameters.EnableAutoDepthStencil = TRUE; // DX가 자동적으로 깊이 / 스텐실 버퍼를 만들고 관리하기 원하는가
	Present_Parameters.AutoDepthStencilFormat = D3DFMT_D24S8; // 깊이 버퍼와 스테실 버퍼의 포맷 설정

	Present_Parameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; 	// 전체 화면 시에는 그래픽 카드가 현재 모니터 재생률을 토대로 수행

																				// 모니터 재생률과 시연(프로그램 프레임)의 간격 설정

																				// D3DPRESENT_INTERVAL_IMMEDIATE : 즉시 시연
																				// D3DPRESENT_INTERVAL_DEFAULT : 적절한 간격을 DX가 알아서 결정, 보통 모니터 재생률을 따라감

	Present_Parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// CreateDevice : 그래픽 장치를 제어할 객체(m_pDevice)를 생성하는 함수

	// 1. 어떤 그래픽 카드를 제어할 것인가
	// 2. 어떤 정보로 장치에 접근할 것인가
	// 3. 장치를 사용할 윈도우 핸들
	// 4. 동작 방식
	// 5. 사용 환경
	// 6. 생성할 객체의 주소값을 저장하기 위한 포인터
	FAILED_CHECK_RETURN(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwFlag, &Present_Parameters, &m_pGraphicDev), E_FAIL);

	*ppGraphicDev = this;

	return S_OK;
}

void CGraphicDev::Render_Begin(D3DXCOLOR Color)
{
	m_pGraphicDev->Clear(0, // 렉트의 개수
		NULL, // 렉트 배열의 첫 번째 주소
		D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, // 비우고자하는 버퍼
															   // 버퍼 중 하나라도 값이 있으면 싹 지워라....
		Color,  // 후면버퍼 색상(0~1사이의 색상에 대한 원시자료값)
		1.f, // z버퍼 초기화 값 (범위 0.f ~ 1.f  // 1이 가장 깊다.)
		0); // 스텐실 버퍼 초기화 값
	m_pGraphicDev->BeginScene();// 여기서부터 후면 버퍼에 그린다.
}

void CGraphicDev::Render_End(HWND hWnd)
{
	m_pGraphicDev->EndScene();// 여기까지가 후면 버퍼에 다 그린 시점

							  // 후면 버퍼와 전면 버퍼를 교환하여 화면에 출력

							  // 1,2번 인자 : 스왑체인 옵션이 D3DSWAPEFFECT_COPY로 작성하지 않은 한 NULL
							  // 3번 인자 : 출력 대상 윈도우 핸들, NULL인 경우 Set_Parameters에서 지정한 핸들이 기본 값으로 설정됨
							  // 4번 인자 : 스왑체인 옵션이 D3DSWAPEFFECT_COPY로 작성하지 않은 한 NULL
							  // hWnd : 어떤 창을 받아 처리할 건지에 따라 다르다.(나중에 미니맵같은거 생길때) 

	m_pGraphicDev->Present(nullptr, nullptr, hWnd, nullptr);
}

void CGraphicDev::Free(void)
{
	_ulong	dwRefCnt = 0;

	if (dwRefCnt = Safe_Release<LPDIRECT3DDEVICE9>(m_pGraphicDev))
		MSG_BOX("m_pGraphicDev Release Failed");

	if (dwRefCnt = Safe_Release<LPDIRECT3D9>(m_pSDK))
		MSG_BOX("m_pSDK Release Failed");
}
