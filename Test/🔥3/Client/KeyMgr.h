#pragma once

#include "Include.h"

class CKeyMgr
{
	DECLARE_SINGLETON(CKeyMgr)
private:
	CKeyMgr();
	~CKeyMgr();

public:

public:
	bool		Key_Pressing(int _iKey);
	bool		Key_Down(int _iKey);	// 눌렀을 때 단 한 번만 키 값이 발생
	bool		Key_Up(int _iKey);		// 눌렀다가 뗐을 때 한 번만 키 값이 발생

	void		Clear_KeyState();

private:
	bool				m_bKeyState[VK_MAX];
};

