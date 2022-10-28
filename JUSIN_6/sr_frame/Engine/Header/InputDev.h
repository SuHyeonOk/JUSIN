#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	E_END
};

enum
{
	KEY_TYPE_COUNT = static_cast<_int>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<_int>(KEY_STATE::E_END),
};

class ENGINE_DLL CInputDev :	public CBase
{
	DECLARE_SINGLETON(CInputDev)

private:
	explicit CInputDev();
	virtual ~CInputDev();

public:
	_byte		Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID)
	{
		return m_MouseState.rgbButtons[byMouseID];
	}
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
	{
		return *(((long*)&m_MouseState) + eMoveState);
	}

	// 누르고 있을 때
	bool DIKeyPressing(_ubyte byKeyID) { return _states[byKeyID] == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool DIKeyDown(_ubyte byKeyID) { return _states[byKeyID] == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool DIKeyUp(_ubyte byKeyID) { return _states[byKeyID] == KEY_STATE::UP; }

	bool MouseKeyDown(MOUSEKEYSTATE byMouseID)
	{
		if (byMouseID == DIM_LB)
		{
			return m_MouseLeftState == KEY_STATE::DOWN;
		}
		else if (byMouseID == DIM_RB)
		{
			return m_MouseRightState == KEY_STATE::DOWN;
		}
		return false;
	};
	bool MouseKeyUp(MOUSEKEYSTATE byMouseID)
	{
		if (byMouseID == DIM_LB)
		{
			return m_MouseLeftState == KEY_STATE::UP;
		}
		else if (byMouseID == DIM_RB)
		{
			return m_MouseRightState == KEY_STATE::UP;
		}
		return false;
	}
	bool MouseKeyPress(MOUSEKEYSTATE byMouseID)
	{
		if (byMouseID == DIM_LB)
		{
			return m_MouseLeftState == KEY_STATE::PRESS;
		}
		else if (byMouseID == DIM_RB)
		{
			return m_MouseRightState == KEY_STATE::PRESS;
		}
		return false;
	}

public:
	HRESULT			Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void			SetUp_InputDev(void);

private:
	_byte			m_byKeyState[256];
	DIMOUSESTATE	m_MouseState;

	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

	LPDIRECTINPUT8				m_pInputSDK;

	HWND m_hWnd;
	vector<KEY_STATE> _states;
	KEY_STATE m_MouseLeftState;
	KEY_STATE m_MouseRightState;
public:
	virtual void		Free(void);
};

END