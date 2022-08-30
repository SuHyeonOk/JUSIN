#include "TimerMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimerMgr)

CTimerMgr::CTimerMgr()
{
}


CTimerMgr::~CTimerMgr()
{
	Free(); // ������ �̱� ������ Freeȣ��
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
	// �������̳� �߰� �뵵
	CTimer*	pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)		// �ش� Ű���� �ִ�
		return E_FAIL;

	pTimer = CTimer::Create();	// Ű ���� NULL�̶�� Ÿ�̸� ����
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	// ������ �ʱ�ȭ�� �̿��ϸ� m_mapTimers�� �־���
	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}
void Engine::CTimerMgr::Update_Timer(const _tchar* pTimerTag)
{
	// Set�� ���
	CTimer*	pTimer = Find_Timer(pTimerTag);
	NULL_CHECK(pTimer);

	pTimer->Update_Timer();
}
_float Engine::CTimerMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	// ã�Ƽ� ��ȯ
	CTimer*	pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.f);

	return pTimer->Get_TimeDelta();
}
