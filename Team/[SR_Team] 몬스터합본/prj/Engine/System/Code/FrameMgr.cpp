#include "..\..\Header\FrameMgr.h"

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
	NULL_CHECK_RETURN(pFrame, false);	// 없으면 return false

	return pFrame->IsPermit_Call(fTimeDelta);
}

HRESULT CFrameMgr::Ready_Frame(const _tchar * pFrameTag, const _float & fCallLimit)
{
	CFrame*		pFrame = Find_Frame(pFrameTag);
	if (nullptr != pFrame)
		return E_FAIL;

	pFrame = CFrame::Create(fCallLimit);	// 몇프레임 제한할 건지 지정
	NULL_CHECK_RETURN(pFrame, E_FAIL);

	m_mapFrame.insert({ pFrameTag, pFrame });

	return S_OK;
}

CFrame * CFrameMgr::Find_Frame(const _tchar * pFrameTag)
{
	auto iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrameMgr::Free(void)
{
	// 결국 내부를 Release 해주는 역할을 하고 있네
	for_each(m_mapFrame.begin(), m_mapFrame.end(), CDeleteMap());
	m_mapFrame.clear();

}
