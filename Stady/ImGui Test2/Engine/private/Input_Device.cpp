#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr), m_pKeyBoard(nullptr), m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
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
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Invalidate_Input_Device(void)
{
	memcpy(&m_preKeyState, &m_byKeyState, sizeof(_char) * 256);
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