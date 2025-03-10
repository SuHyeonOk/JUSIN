#include "..\public\RenderTarget.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 * pClearColor)
{
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	/* 장치에 바인딩하여 셰이더를 통해 바인딩된 이 텍스쳐에 픽셀을 기록한다.*/
	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	/* 이 텍스쳐를 셰이더 전역으로 전달하여 픽셀셰이더안에서 이 텍스쳐에 저장된 픽셀의 값(Diffuse, Normal, Shade)을 읽어온다. */
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = *pClearColor;

	return S_OK;
}

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 * pClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, pClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{

	if(m_vClearColor.y == 0.f)
		SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Test.dds"));

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);
}
