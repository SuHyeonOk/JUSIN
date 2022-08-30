#include "GraphicDev.h"

USING(Engine)
IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
	: m_pGraphicDev(nullptr)
	, m_pSDK(nullptr)
{
}


CGraphicDev::~CGraphicDev()
{
	Free();
}


HRESULT CGraphicDev::Ready_GraphicDev(HWND hWnd, WINMODE eMode, 
									const _uint & iSizeX, const _uint & iSizeY,
									CGraphicDev** ppGraphicDev)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	NULL_CHECK_RETURN(m_pSDK, E_FAIL);


	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	FAILED_CHECK_RETURN(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps), E_FAIL);

	_ulong	dwFlag = 0;

	// 정점연산? 하드웨어 연산? 으로 할지
	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	else
		dwFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS		Present_Parameters;
	ZeroMemory(&Present_Parameters, sizeof(D3DPRESENT_PARAMETERS));

	// 디바이스 장치자 자동으로 백버터 생성
	// 백 버퍼란 텍스처(픽셀들이 색상값을 모아 놓은 파일) 이다 

	// 화면 갱신 옵션들 (이전에는 함수를 이용했었음)
	Present_Parameters.BackBufferWidth = iSizeX;
	Present_Parameters.BackBufferHeight = iSizeY;
	Present_Parameters.BackBufferFormat = D3DFMT_A8R8G8B8; // 각각 8비트씩 총 32비트 텍스처 형태
	Present_Parameters.BackBufferCount = 1; // 백 버퍼 개수

	Present_Parameters.MultiSampleType = D3DMULTISAMPLE_NONE; // 안티에일리어싱 (계단 형산 방지)
	Present_Parameters.MultiSampleQuality = 0;

	//D3DSWAPEFFECT_DISCARD: 스왑 체인
	//D3DSWAPEFFECT_FLIP : 버퍼 하나로 뒤집으면서 사용하는 방식
	//D3DSWAPEFFECT_COPY : 더블 버퍼링과 유사한 복사 방식

	//더블 버퍼링은 우리가 API일 때 깜박거림을 방지하기 위해 사용했던 방법이다
	//화면버퍼와 똑같은 텍스처를 만들어서 최종적인 화면 이미지를 복사하여 최종적인 이미지를 화면 버퍼 위에 복사하는 방식
	//을 다이렉트 에서는 DISCARD를 이용하여 깜박거림을 방지한다
	//왜 스왑체인으로 바꿨을까 ?
	//-더블 버퍼링의 단점은 이전에 그린 화면을 지우지 않고 그 위에 계속 덮어씌운다
	//그런데 다이렉트에서 이렇게 덮어씌우면 속도를 감당하지 못 한다 그래서 이전 이미지를 지워야 한다
	Present_Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD; 
	Present_Parameters.hDeviceWindow = hWnd; // 장치를 사용할 윈도우 핸들 지정

	Present_Parameters.Windowed = eMode; // MODE_FULL(false)전체화면, MODE_WIN(true)창 모드

	// DX가 자동적으로 깊이 / 스텐실 버퍼를 만들고 관리하기 원하는가
	Present_Parameters.EnableAutoDepthStencil = TRUE;
	// 깊이 버퍼와 스테실 버퍼의 포맷 설정
	Present_Parameters.AutoDepthStencilFormat = D3DFMT_D24S8;

	// 전체 화면 시에는 그래픽 카드가 현재 모니터 재생률을 토대로 수행
	Present_Parameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	// D3DPRESENT_INTERVAL_IMMEDIATE : 즉시 시연
	// D3DPRESENT_INTERVAL_DEFAULT : 적절한 간격을 DX가 알아서 결정, 보통 모니터 재생률을 따라감
	Present_Parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	FAILED_CHECK_RETURN(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwFlag, &Present_Parameters, &m_pGraphicDev), E_FAIL);

	// ppGraphicDev : 이 class의 더블포인터형 이다(컴객체X) 
	// 외부에서 이 함수를 통해 디바이스 초기화 끝난 형태의 객체를 받기위한 output용도이다
	*ppGraphicDev = this;

	return S_OK;
}

void CGraphicDev::Render_Begin(D3DXCOLOR Color)
{
	// 후면 버퍼 지우기
	// Color : 0 ~ 1
	// 1.f : 깊이버퍼 초기화 값
	m_pGraphicDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, Color, 1.f, 0);
	m_pGraphicDev->BeginScene();
}

void CGraphicDev::Render_End(HWND hWnd)
{
	m_pGraphicDev->EndScene();
	m_pGraphicDev->Present(nullptr, nullptr, hWnd, nullptr);
}

void Engine::CGraphicDev::Free(void)
{
	_ulong	dwRefCnt = 0;

	if (dwRefCnt = Safe_Release<LPDIRECT3DDEVICE9>(m_pGraphicDev))
		MSG_BOX("m_pGraphicDev Release Failed");

	// 디버깅시를 위해 Safe_Release
	if (dwRefCnt = Safe_Release<LPDIRECT3D9>(m_pSDK))
		MSG_BOX("m_pSDK Release Failed");
}
