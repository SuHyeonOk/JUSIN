#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

// DInput(Direct���� �����ϴ� Ű���� �� ���콺 �Է� ü��)�� �̿��� �Էµ���̽�
// direct�� �����ϴ� Űó�� ����� ���� ������ ��, ���� ����� �ƴϴ�.
class ENGINE_DLL CInputDev : public CBase
{
	DECLARE_SINGLETON(CInputDev)

private:
	explicit CInputDev();
	virtual ~CInputDev();

public:
	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEYSTATE byMouseID) { return m_MouseState.rgbButtons[byMouseID]; }
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMoveState) { return *(((long*)&m_MouseState) + eMoveState); }

public:
	HRESULT		Ready_InputDev(HINSTANCE hInst, HWND hWnd); 
	void		SetUp_InputDev(void);

private:
	_byte			m_byKeyState[256];
	DIMOUSESTATE	m_MouseState;
		// z : ��  // rgbButtons[4] : xbutton(?)�̶�� ���� �ν��ϱ� ���� �迭ũ�Ⱑ 4��� ��.
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;
	LPDIRECTINPUT8				m_pInputSDK;

public:
	virtual void		Free(void);
};

END