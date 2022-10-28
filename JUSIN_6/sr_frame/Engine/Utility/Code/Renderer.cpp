#include "..\..\Header\Renderer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}


CRenderer::~CRenderer()
{
}

void CRenderer::Add_RenderGroup(RENDERID eID, CGameObject * pGameObject)
{
	if (eID >= RENDER_END || nullptr == pGameObject)
		return;

	m_RenderGroup[eID].push_back(pGameObject);
	pGameObject->AddRef();
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (_uint i = 0; i < RENDER_UI; ++i)
	{
		for (auto& iter : m_RenderGroup[i])
		{
			iter->Render_Object();
			Safe_Release(iter);			// 삭제가 아님, 레퍼런스 카운트 감소
		}
		m_RenderGroup[i].clear();
	}

	RenderUI(pGraphicDev);


}

void CRenderer::Clear_RenderGroup(void)
{

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::RenderUI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	_matrix			OldViewMatrix, OldProjMatrix;
	pGraphicDev->GetTransform(D3DTS_VIEW, &OldViewMatrix);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &OldProjMatrix);

	m_RenderGroup[RENDER_UI].sort([](CGameObject* a, CGameObject* b)
	{
		return a->m_iRenderPriority > b->m_iRenderPriority;
	});
	
	for (auto& iter : m_RenderGroup[RENDER_UI])
	{
		iter->Render_Object();

		Safe_Release(iter);			// 삭제가 아님, 레퍼런스 카운트 감소
	}
	m_RenderGroup[RENDER_UI].clear();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pGraphicDev->SetTransform(D3DTS_VIEW, &OldViewMatrix);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &OldProjMatrix);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

}

void Engine::CRenderer::Free(void)
{
	Clear_RenderGroup();
	CComponent::Free();
}
