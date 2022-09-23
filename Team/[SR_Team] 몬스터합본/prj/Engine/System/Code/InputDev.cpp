#include "..\..\Header\InputDev.h"

#include "Export_Function.h"

USING(Engine)
IMPLEMENT_SINGLETON(CInputDev)

CInputDev::CInputDev()
	: m_pInputSDK(nullptr), m_pKeyBoard(nullptr), m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
}


CInputDev::~CInputDev()
{
	Free();
}

HRESULT CInputDev::Ready_InputDev(HINSTANCE hInst, HWND hWnd) 
{
	// m_pInputSDK 를 통해 m_pKeyboard, m_pMouse를 생성한다.

	// DInput 컴객체를 생성하는 함수
	FAILED_CHECK_RETURN(DirectInput8Create(hInst,
								DIRECTINPUT_VERSION,
								IID_IDirectInput8,
								(void**)&m_pInputSDK,
								NULL), E_FAIL);

	// 키보드 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
									// gxguid.lib에 다양한 옵션 상수값에 대한 정보를 가지고 있다.

	// 장치에 대한 access 버전을 받아오는 함수
	// 앞으로 키보드에 대한 접근은 이 멤버변수를 통해 하겠다는 권한을 부여받음.
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;
}

void CInputDev::SetUp_InputDev(void)
{
	// 매프레임마다 이 함수를 통해 상태 정보를 받는다.
	// m_pKeyboard, m_pMouse를 통해서 입력을 받고,
	// 키보드에서 발생한 메시지 > m_byKeyState에,
	// 마우스에서 발생한 메시지 > m_MouseState에 저장

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void CInputDev::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
