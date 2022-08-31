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
	NULL_CHECK_RETURN(pFrame, false); // ������ false
	
	return pFrame->IsPermit_Call(fTimeDelta); // Ű ���� ���� ã���� ����Լ� ȣ��
}

HRESULT CFrameMgr::Ready_Frame(const _tchar * pFrameTag, const _float & fCallLimit)
{
	CFrame*		pFrame = Find_Frame(pFrameTag);

	// Ű���� ���� ã�Ҵµ� �־����� ���� ����� ���� �ȵǴ� return
	if (nullptr != pFrame)
		return E_FAIL;

	pFrame = CFrame::Create(fCallLimit); 
	NULL_CHECK_RETURN(pFrame, E_FAIL);		  // Ű ���� ���� ã�Ҵµ� ������ 		

	m_mapFrame.insert({ pFrameTag, pFrame }); // m_mapFrame�� �־��ش�

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
