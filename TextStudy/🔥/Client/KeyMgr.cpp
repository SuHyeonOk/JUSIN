#include "stdafx.h"
#include "KeyMgr.h"

CKeyMgr*	CKeyMgr::m_pInstance = nullptr;

CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
}

CKeyMgr::~CKeyMgr()
{

}

bool CKeyMgr::Key_Pressing(int _iKey)
{
	if (GetAsyncKeyState(_iKey) & 0x8000)
		return true;
	
	return false;
}

bool CKeyMgr::Key_Down(int _iKey)
{
	// 이전에 눌린 적이 없고, 현재 눌렸다고 값이 발생했을 경우

	if (!m_bKeyState[_iKey] && GetAsyncKeyState(_iKey) & 0x8000)
	{
		m_bKeyState[_iKey] = true;
		return true;
	}

	for (int i = 0; i < VK_MAX; ++i)
	{
		if (m_bKeyState[i] && !(GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = false;
	}

	return false;
}

bool CKeyMgr::Key_Up(int _iKey)
{
	if (m_bKeyState[_iKey] && !(GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = false;
		return true;
	}

	for (int i = 0; i < VK_MAX; ++i)
	{
		if (!m_bKeyState[i] && (GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = true;
	}

	return false;
}
