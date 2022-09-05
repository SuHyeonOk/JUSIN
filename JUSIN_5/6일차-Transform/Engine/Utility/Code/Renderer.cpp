#include "..\..\Header\Renderer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}


CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eID, CGameObject * pGameObject)
{
	if (eID >= RENDER_END || nullptr == pGameObject) // 예외처리
		return;

	m_RenderGroup[eID].push_back(pGameObject);
	pGameObject->AddRef(); // stl 컨테이너에 원소를 복사해서 넣기에
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& iter : m_RenderGroup[i])
		{
			iter->Render_Obejct();
			Safe_Release(iter);			// 삭제가 아님, 레퍼런스 카운트 감소
		}
		m_RenderGroup[i].clear();
	}

}

void CRenderer::Clear_RenderGroup(void) // ﻿나중에 필요에 의해 외부에서 전체 삭제를 위해서 만들어 둠
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void Engine::CRenderer::Free(void)
{
	Clear_RenderGroup();
}
