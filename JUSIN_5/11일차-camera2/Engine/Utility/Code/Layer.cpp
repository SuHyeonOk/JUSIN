#include "..\..\Header\Layer.h"

USING(Engine)

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pObjTag, const _tchar * pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second->Get_Component(pComponentTag, eID);
}

HRESULT CLayer::Add_GameObject(const _tchar * pObjTag, CGameObject * pInstance)
{
	if (nullptr == pInstance)
		return E_FAIL;

	m_mapObject.insert({ pObjTag, pInstance });

	return S_OK;
}

HRESULT CLayer::Ready_Layer(void)
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	_int iResult = 0;

	for (auto& iter : m_mapObject)
	{
		iResult = iter.second->Update_Object(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	return iResult;
}

void CLayer::LateUpdate_Layer(void)
{
	for (auto& iter : m_mapObject)
		iter.second->LateUpdate_Object();
}

CLayer* CLayer::Create(void)
{
	CLayer*	pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
		Safe_Release(pLayer);

	return pLayer;
}

void CLayer::Free(void)
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}
