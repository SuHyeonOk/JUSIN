#include "..\..\Header\TargetTexture.h"
#include "Shader.h"
#include "GameUtilMgr.h"

CTargetTexture::CTargetTexture(LPDIRECT3DDEVICE9 pGraphicDev) : CComponent(pGraphicDev)
{
}

CTargetTexture::CTargetTexture(const CTargetTexture& rhs) : CComponent(rhs)
{
	m_pTexture = rhs.m_pTexture;
	m_pTexture->AddRef();
	m_pRender2Surface = rhs.m_pRender2Surface;
	m_pRender2Surface->AddRef();
}

CTargetTexture::~CTargetTexture()
{
}

HRESULT CTargetTexture::Ready_Texture()
{
	FAILED_CHECK(m_pGraphicDev->CreateTexture(WINCX, WINCY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, nullptr));

	FAILED_CHECK(D3DXCreateRenderToSurface(m_pGraphicDev, WINCX, WINCY, D3DFMT_A8R8G8B8, FALSE, D3DFMT_UNKNOWN, &m_pRender2Surface));

	m_pMvpTexture = CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/MVP/mvp_%d.png", TEX_NORMAL, 1);
	m_pBuff = CRcTex::Create(m_pGraphicDev);
	return S_OK;
}

void CTargetTexture::Set_Texture(const _uint& iIndex)
{
	m_pGraphicDev->SetTexture(0, m_pTexture);
}

void CTargetTexture::Set_Texture(CShader* pShaderCom, D3DXHANDLE hHandle, const _uint& iIndex)
{
	pShaderCom->Set_Texture(hHandle, m_pTexture);
}

void CTargetTexture::RenderOnSurface()
{
	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	IDirect3DSurface9* pSurface;
	FAILED_CHECK_RETURN(m_pTexture->GetSurfaceLevel(0, &pSurface));

	m_pRender2Surface->BeginScene(pSurface, &ViewPort);
	m_pGraphicDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(130, 0, 0, 0), 1.f, 0);


	_matrix matTexWorld;
	CGameUtilMgr::MatWorldComposeEuler(matTexWorld, {10.5f, 6.f, 1.f}, {0.f, D3DXToRadian(180.f), 0.f}, {0.f ,0.f, 0.f});
	 m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	 m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	 m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	 m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	 m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	 m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTexWorld);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pMvpTexture->Set_Texture();
	m_pBuff->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
   m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
   m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	for (auto e : m_RenderList)
	{	if (e)
			e->Render_Object();
	}

	m_pRender2Surface->EndScene(D3DX_FILTER_LINEAR);

	Safe_Release(pSurface);
	m_RenderList.clear();
}

CTargetTexture* CTargetTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTargetTexture* pInstance = new CTargetTexture(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture()))
	{
		MSG_BOX("Texture Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* CTargetTexture::Clone()
{
	_CRASH("Dont Clone This");
	return nullptr;
}

void CTargetTexture::Free()
{
	Safe_Release(m_pMvpTexture);
	Safe_Release(m_pBuff);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRender2Surface);
	CComponent::Free();
}


