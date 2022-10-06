#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

// DInput(Direct에서 제공하는 키보드 및 마우스 입력 체계)을 이용한 입력디바이스
// direct가 제공하는 키처리 방법을 배어보기 위함일 뿐, 좋은 기능은 아니다.
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
		// z : 휠  // rgbButtons[4] : xbutton(?)이라는 것을 인식하기 위해 배열크기가 4라고 함.
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;
	LPDIRECTINPUT8				m_pInputSDK;

public:
	virtual void		Free(void);
};

END