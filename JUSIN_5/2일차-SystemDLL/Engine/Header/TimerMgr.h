#pragma once

#include "Timer.h"

BEGIN(Engine)

class ENGINE_DLL CTimerMgr : public CBase
{
	DECLARE_SINGLETON(CTimerMgr)

private:
	explicit CTimerMgr();
	virtual ~CTimerMgr();

public:
	_float			Get_TimeDelta(const _tchar* pTimerTag);


public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);
	void		Update_Timer(const _tchar* pTimerTag);

private:
	CTimer*		Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, CTimer*>		m_mapTimers; // key값으로 유니코드 문자 상수

public:
	virtual void Free(void);
};

END