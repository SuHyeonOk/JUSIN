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
	bool		Key_Down(int _iKey);	// ������ �� �� �� ���� Ű ���� �߻�
	bool		Key_Up(int _iKey);		// �����ٰ� ���� �� �� ���� Ű ���� �߻�

	void		Clear_KeyState();

private:
	bool				m_bKeyState[VK_MAX];
};

