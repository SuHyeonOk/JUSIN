#include "..\public\Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || 
		eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject); // 게임 오브젝트 추가

	Safe_AddRef(pGameObject); // 레퍼런스 카운트 증가

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	/*
	LPDIRECT3DDEVICE9		pDevice = nullptr;

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// 알파 테스트를 킨다.
	pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// 0 ~ 255값을 지정한다.
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	*/
	
	/*
	LPDIRECT3DDEVICE9		pDevice = nullptr;

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);			// 알파 블렌딩을 켠다.
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// (더하기로)섞는다.
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// 어떻게 더하기를 하는지
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	_float4			vSourColor, vDestColor;
	// vSourColor : 내가 지금 막 그리려고 한 픽셀의 색
	// vDestColor : 내가 픽셀을 그리고자하는 위치에 이미 기록되어 있던 색

	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
*/
	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRenderer * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}
void CRenderer::Free()
{
	__super::Free();

}
