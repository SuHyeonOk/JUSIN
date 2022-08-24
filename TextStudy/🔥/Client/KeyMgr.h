#pragma once

#include "Include.h"

class CKeyMgr
{
private:
	CKeyMgr();
	~CKeyMgr();

public:
	static CKeyMgr*		Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CKeyMgr;

		return m_pInstance;
	}
	static void		Destroy_Instance(void)
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

public:
	bool		Key_Pressing(int _iKey);
	bool		Key_Down(int _iKey);	// 눌렀을 때 단 한 번만 키 값이 발생
	bool		Key_Up(int _iKey);		// 눌렀다가 뗐을 때 한 번만 키 값이 발생

private:
	static CKeyMgr*		m_pInstance;
	bool				m_bKeyState[VK_MAX];
};

