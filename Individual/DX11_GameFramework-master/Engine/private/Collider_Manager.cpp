#include "..\public\Collider_Manager.h"

#include "Collider.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollider_Manager)


CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Add_ColGroup(COL_TYPE eColType, CGameObject * pGameObject)
{
	if (nullptr == pGameObject ||
		eColType >= COL_END)
		return E_FAIL;

	m_ColliderObjects[eColType].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

void CCollider_Manager::Update_Col(COL_TYPE eColType_L, COL_TYPE eColType_R)
{

	for (auto& pObj_L : m_ColliderObjects[eColType_L])
	{
		for (auto& pObj_R : m_ColliderObjects[eColType_R])
		{

			CCollider* pCol_L = (CCollider*)pObj_L->Get_ComponentPtr(TEXT("Com_Collider"));
			CCollider* pCol_R = (CCollider*)pObj_R->Get_ComponentPtr(TEXT("Com_Collider"));

			if (pCol_L->Collision(pCol_R))
			{
				pObj_L->On_Collision(pObj_R);
				pObj_R->On_Collision(pObj_L);
			}
		}
	}

}

void CCollider_Manager::Reset_Col()
{
	for (_uint i = 0; i < COL_END; ++i)
	{
		for (auto& pObj : m_ColliderObjects[i])
			Safe_Release(pObj);
		m_ColliderObjects[i].clear();
	}

}

void CCollider_Manager::Free()
{
	Reset_Col();
}
