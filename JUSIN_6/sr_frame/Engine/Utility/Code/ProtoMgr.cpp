#include "..\..\Header\ProtoMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CProtoMgr)

CProtoMgr::CProtoMgr()
{
}


CProtoMgr::~CProtoMgr()
{
	Free();
}

HRESULT CProtoMgr::Ready_Proto(const wstring& pProtoTag, CComponent * pComponent)
{
	CComponent*		pPrototype = Find_Proto(pProtoTag);

	if (nullptr != pPrototype || pProtoTag.empty())
		return E_FAIL;

	m_mapProto.insert({ pProtoTag, pComponent });

	return S_OK;
}

CComponent * CProtoMgr::Clone_Proto(const wstring& pProtoTag)
{
	CComponent*		pPrototype = Find_Proto(pProtoTag);

	if (pProtoTag.empty())
		return nullptr;

	NULL_CHECK_RETURN(pPrototype, nullptr);

	return pPrototype->Clone();
}

CComponent * CProtoMgr::Find_Proto(const wstring& pProtoTag)
{
	// auto	iter = find_if(m_mapProto.begin(), m_mapProto.end(), CTag_Finder(pProtoTag));
	auto iter = m_mapProto.find(pProtoTag);
	if (iter == m_mapProto.end())
		return nullptr;
	
	return iter->second;
}

void Engine::CProtoMgr::Free(void)
{
	for_each(m_mapProto.begin(), m_mapProto.end(), CDeleteMap());
	m_mapProto.clear();
}
