#include "..\public\Target_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 * pClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iWidth, iHeight, ePixelFormat, pClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);

	if(nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)				// Ư�� ���� Ÿ���� ���� ��������� ��Ȳ���� �׳� ����� �ָ� �ȴ�.
	{
		list<CRenderTarget*>		MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else									// List�� �̹� �ִ� ����, �� ����Ʈ�� ���� Ÿ���� �߰��Ѵ�.
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	// ������ �׸��� Target �� �������� �� (�ٸ� ��� ���δ� Get���� ���ٽ� �Լ��� ���� �Ǳ��ϴ�.)
	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRTVs[8] = { nullptr };

	_uint			iNumViews = 0;

	for (auto& pRTV : *pMRTList)	
		pRTVs[iNumViews++] = pRTV->Get_RTV();	// RenderTarget �� �ִ� ��+����Ʈ�� �����´�.

	pContext->OMSetRenderTargets(iNumViews, pRTVs, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	// ���� �� �ϴ� ���� �ʱ�ȭ �ϴ� �� �̴�. 
	pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)		
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();
}
