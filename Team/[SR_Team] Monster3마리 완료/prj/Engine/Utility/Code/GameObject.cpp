#include "..\..\Header\GameObject.h"

USING(Engine)


CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag, COMPONENTID eID)
{
	CComponent*		pComponent = Find_Component(pComponentTag, eID);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Ready_Object(void)
{
	return S_OK;
}

_int CGameObject::Update_Object(const _float & fTimeDelta)
{
	// 매프레임마다 변환되어야 할 컴포넌트는 dynamic으로 저장 될 것임
	// ex) 행렬변환을 하는 컴포넌트
	// tricol, rcCol처럼 버퍼로 그려서 변환없이 그대로 가는 거라면 static
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->Update_Component(fTimeDelta);

	return 0;
}

void CGameObject::LateUpdate_Object(void)
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->LateUpdate_Component();
}

void CGameObject::Render_Obejct(void)
{
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag, COMPONENTID eID)
{
	auto iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));

	if (iter == m_mapComponent[eID].end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free(void)
{
	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}
