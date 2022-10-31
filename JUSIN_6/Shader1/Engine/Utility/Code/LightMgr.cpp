#include "..\..\Header\LightMgr.h"

#include "Export_Function.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{
}


CLightMgr::~CLightMgr()
{
	Free();
}

HRESULT Engine::CLightMgr::Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	CLight*		pLight = CLight::Create(pGraphicDev, pLightInfo, iIndex);
	NULL_CHECK_RETURN(pLight, E_FAIL);

	m_LightList.push_back(pLight);

	return S_OK;
}

void Engine::CLightMgr::Free(void)
{
	for_each(m_LightList.begin(), m_LightList.end(), CDeleteObj());
	m_LightList.clear();
}
