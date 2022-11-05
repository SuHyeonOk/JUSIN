#include "..\public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))		// 이미 원형 객체가 있어
		return E_FAIL;									// 추가 하면 안 돼

	m_Prototypes.emplace(pPrototypeTag, pPrototype);	// 원형 객체가 없다면 추가해
	
	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
// 원형 객체를 추가하기 위해 pArg 매개변수 추가
// m_pLayers배열을 몇개 할당 하기 위한 iLevelIndex 매개 변수 추가
{
	
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL; // 원형이 없다면 return

	CGameObject*		pGameObject = pPrototype->Clone(pArg); // 원형 객체의 Clone호출

	CLayer*		pLayer = Find_Layer(pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create(pGameObject);
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pPrototype);

	return S_OK;
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));
	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(const _tchar * pLayerTag)
{
	//auto	iter = find_if(m_pLayers.begin(), m_pLayers.end(), CTag_Finder(pLayerTag));
	//if (iter == m_pLayers.end())
	//	return nullptr;

	//return iter->second;
	return S_OK;
}

void CObject_Manager::Free()
{
}

