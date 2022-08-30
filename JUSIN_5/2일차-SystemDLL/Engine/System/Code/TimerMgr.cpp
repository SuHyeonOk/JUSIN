#include "TimerMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimerMgr)

CTimerMgr::CTimerMgr()
{
}


CTimerMgr::~CTimerMgr()
{
	Free(); // 생성자 이기 때문에 Free호출
}

void Engine::CTimerMgr::Free(void)
{
	for_each(m_mapTimers.begin(), m_mapTimers.end(), CDeleteMap());
	m_mapTimers.clear();
}

CTimer* Engine::CTimerMgr::Find_Timer(const _tchar* pTimerTag)
{
	auto		iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return nullptr;
	
	return iter->second;
}

HRESULT Engine::CTimerMgr::Ready_Timer(const _tchar* pTimerTag) 
{
	// 맵컨테이너 추가 용도
	CTimer*	pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)		// 해당 키값이 있다
		return E_FAIL;

	pTimer = CTimer::Create();	// 키 값이 NULL이라면 타이머 생성
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	// 유니폼 초기화를 이용하며 m_mapTimers에 넣어줌
	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}
void Engine::CTimerMgr::Update_Timer(const _tchar* pTimerTag)
{
	// Set과 비슷
	CTimer*	pTimer = Find_Timer(pTimerTag);
	NULL_CHECK(pTimer);

	pTimer->Update_Timer();
}
_float Engine::CTimerMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	// 찾아서 반환
	CTimer*	pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.f);

	return pTimer->Get_TimeDelta();
}
