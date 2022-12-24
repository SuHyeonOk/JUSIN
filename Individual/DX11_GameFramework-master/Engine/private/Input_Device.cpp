#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr), m_pKeyBoard(nullptr), m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
}

_int CInput_Device::Key_DwonUp(_uchar eKeyID)
{
	// 1 : 지금 한 번 눌렀다. / 0 : 지금 두번째 눌렀다. / 2 : 아무것도 아닌 상태 / 3 : 이전에 누른키다.

	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_byKeyState[eKeyID] & 0x80)
			m_DownCount++;
	}

	if (1 == m_DownCount)
		return 1;
	else if (2 == m_DownCount)
	{
		m_DownCount = 0;
		return 0;
	}
}

_bool CInput_Device::Key_Down(_uchar eKeyID)
{
	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_byKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Key_Up(_uchar eKeyID)
{
	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Key_Pressing(_uchar eKeyID)
{
	if (m_preKeyState[eKeyID] == m_byKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Mouse_Down(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_MouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Mouse_Up(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_PreMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] == m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_MouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;


	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Invalidate_Input_Device(void)
{
	memcpy(&m_preKeyState, &m_byKeyState, sizeof(_byte) * 256);
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	memcpy(&m_PreMouseState, &m_MouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
