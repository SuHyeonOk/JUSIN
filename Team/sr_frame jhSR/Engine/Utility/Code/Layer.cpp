#include "..\..\Header\Layer.h"

USING(Engine)

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent * CLayer::Get_Component(const wstring& pObjTag, const wstring& pComponentTag, COMPONENTID eID)
{
	auto	iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second->Get_Component<CComponent>(pComponentTag, eID);
}

CGameObject* CLayer::Get_GameObject(const wstring& pObjTag)
{
	auto	iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second;
}

void CLayer::Get_AllGameObject(const wstring& pObjTag, list<CGameObject*>& outList)
{
	const auto range = m_mapObject.equal_range(pObjTag);
	for (auto it = range.first; it != range.second; ++it)
		outList.push_back(it->second);
}

HRESULT CLayer::Add_GameObject(const wstring& pObjTag, CGameObject * pInstance)
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
	for (auto itr = m_mapObject.begin(); itr != m_mapObject.end();)
	{
		if (OBJ_DEAD == itr->second->Update_Object(fTimeDelta))
		{
			Safe_Release(itr->second);
			itr = m_mapObject.erase(itr);
		}
		else
		{
			++itr;
		} 
	}

	return LAY_NOEVENT;
}

void CLayer::LateUpdate_Layer(void)
{
	for (auto& iter : m_mapObject)
		iter.second->LateUpdate_Object();
}

HRESULT CLayer::Delete_GameObject(const wstring& pObjTag)
{
	auto	iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return E_FAIL;

	_ulong dwCnt = 0;

	dwCnt = iter->second->Release();

	if (dwCnt == 0)
		iter->second = nullptr;
	
	m_mapObject.erase(iter);
	
	return S_OK;
}

CLayer* CLayer::Create(void)
{
	CLayer*	pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free(void)
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}
