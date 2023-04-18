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
	_float			Get_TimeDelta(const wstring& pTimerTag);


public:
	HRESULT		Ready_Timer(const wstring& pTimerTag);
	void		Update_Timer(const wstring& pTimerTag);

private:
	CTimer*		Find_Timer(const wstring& pTimerTag);

private:
	map<wstring, CTimer*>		m_mapTimers;

public:
	virtual void Free(void);
};

END