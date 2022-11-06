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
	if (nullptr != Find_Prototype(pPrototypeTag))		// �̹� ���� ��ü�� �־�
		return E_FAIL;									// �߰� �ϸ� �� ��

	m_Prototypes.emplace(pPrototypeTag, pPrototype);	// ���� ��ü�� ���ٸ� �߰���
	
	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
// ���� ��ü�� �߰��ϱ� ���� pArg �Ű����� �߰�
// m_pLayers�迭�� � �Ҵ� �ϱ� ���� iLevelIndex �Ű� ���� �߰�
{
	
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL; // ������ ���ٸ� return

	CGameObject*		pGameObject = pPrototype->Clone(pArg); // ���� ��ü�� Cloneȣ��

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

