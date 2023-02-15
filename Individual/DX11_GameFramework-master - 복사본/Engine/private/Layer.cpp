#include "..\public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (list<class CGameObject*>::iterator iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != *iter)
			(*iter)->Tick(TimeDelta);

		// �׾��ٸ�
		if ((*iter)->Get_Dead())
		{
			// �����Ҵ� ����
			Safe_Release(*iter);
			// ����Ʈ���� �̷�����
			iter = m_GameObjects.erase(iter);
		}
		else
			++iter;
	}

	//for (auto& pGameObject : m_GameObjects)
	//{
	//	if (nullptr != pGameObject)
	//		pGameObject->Tick(TimeDelta);
	//}
}

void CLayer::Late_Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(TimeDelta);
	}
}

CComponent * CLayer::Get_ComponentPtr(const _tchar * pComponentTag, _uint iLayerIndex) // ��
{ // �� ����Ʈ�� iLayerIndex ��° ���� pComponentTag ������Ʈ�� ã�Ƽ� ����

	if (iLayerIndex >= m_GameObjects.size())
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (_uint i = 0; i < iLayerIndex; ++i)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Get_ComponentPtr(pComponentTag);
}

CGameObject * CLayer::Get_GameObject(const _tchar * pGameObjectTag, _uint iLayerIndex) // ��
{ // �� ����Ʈ�� iLayerIndex ��° ���� pGameObjTag ������Ʈ�� ã�Ƽ� ����

	if (iLayerIndex >= m_GameObjects.size())
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (_uint i = 0; i < iLayerIndex; ++i)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return *iter;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
