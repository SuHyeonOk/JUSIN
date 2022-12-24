#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_char Get_DIKeyState(_uchar eKeyID) {
		return m_byKeyState[eKeyID];
	}

	_char Get_DIMouseState(MOUSEKEYSTATE eMouseKeyID) {
		return m_MouseState.rgbButtons[eMouseKeyID];
	}

	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID) {
		return ((_long*)&m_MouseState)[eMouseMoveID];
	}

	_int		Key_OneUpDwon(_uchar eKeyID);
	_bool		Key_Down(_uchar eKeyID);
	_bool		Key_Up(_uchar eKeyID);
	_bool		Key_Pressing(_uchar eKeyID);


	_bool		Mouse_Down(MOUSEKEYSTATE eMouseKeyID);
	_bool		Mouse_Up(MOUSEKEYSTATE eMouseKeyID);
	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID);


public:
	HRESULT			Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void			Invalidate_Input_Device(void);

private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

	_char						m_preKeyState[256] = { 0 };
	_char						m_byKeyState[256] = { 0 };
	DIMOUSESTATE				m_PreMouseState;
	DIMOUSESTATE				m_MouseState;

	_int						m_OneUpDown = false;

public:
	virtual void		Free(void);
};

END