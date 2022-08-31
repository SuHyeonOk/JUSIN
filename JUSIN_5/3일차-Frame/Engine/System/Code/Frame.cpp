#include "Frame.h"
#include "Export_Function.h"

USING(Engine)

CFrame::CFrame()
	: m_fCallLimit(0.f), m_fAccTimeDelta(0.f)
{
}


CFrame::~CFrame()
{
	Free();
}

_bool CFrame::IsPermit_Call(const _float & fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta; // fTimeDelta(시간값)이 들어오면 m_fAccTimeDelta에 누적

	if (m_fAccTimeDelta >= m_fCallLimit) // 누적값이 m_fCallLimit(60프레임)보다 커지면
	{
		m_fAccTimeDelta = 0.f; // 중첩시켜두었던 m_fAccTimeDelta 초기화
		return true; 
	}
	// 👉 60번에 1번 꼴당 true를 return할 수 있는 함수
	return false;
}

HRESULT CFrame::Ready_Frame(const _float & fCallLimit)
{
	m_fCallLimit = 1.f / fCallLimit; // fCallLimit에 60이 들어왔다면 1/60

	return S_OK; // 60번에 한 번을 ture로 만드는 것이 목적이다 👉 60프레임
}

CFrame* CFrame::Create(const _float & fCallLimit)
{
	CFrame*		pInstance = new CFrame;

	if (FAILED(pInstance->Ready_Frame(fCallLimit)))
	{
		MSG_BOX("Frame Create Failed");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CFrame::Free(void)
{
}
