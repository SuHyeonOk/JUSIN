#include "FrameMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrameMgr)


CFrameMgr::CFrameMgr()
{
}


CFrameMgr::~CFrameMgr()
{
	Free();
}

_bool CFrameMgr::IsPermit_Call(const _tchar * pFrameTag, const _float & fTimeDelta)
{
	CFrame*		pFrame = Find_Frame(pFrameTag);
	NULL_CHECK_RETURN(pFrame, false); // 없으면 false
	
	return pFrame->IsPermit_Call(fTimeDelta); // 키 값에 의해 찾으면 멤버함수 호출
}

HRESULT CFrameMgr::Ready_Frame(const _tchar * pFrameTag, const _float & fCallLimit)
{
	CFrame*		pFrame = Find_Frame(pFrameTag);

	// 키값에 의해 찾았는데 있었으면 새로 만드는 것이 안되니 return
	if (nullptr != pFrame)
		return E_FAIL;

	pFrame = CFrame::Create(fCallLimit); 
	NULL_CHECK_RETURN(pFrame, E_FAIL);		  // 키 값에 의해 찾았는데 없으면 		

	m_mapFrame.insert({ pFrameTag, pFrame }); // m_mapFrame에 넣어준다

	return S_OK;
}

CFrame * CFrameMgr::Find_Frame(const _tchar * pFrameTag)
{
	auto	iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrameMgr::Free(void)
{
	for_each(m_mapFrame.begin(), m_mapFrame.end(), CDeleteMap());
	m_mapFrame.clear();
}
