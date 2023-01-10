#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {	// 매니저에서 가져가서 보기 위해서
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);

private:
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;

private:
	// 만들어낸 텍스처 (ID3D11Texture2D 는 사실상 ID3D11RenderTargetView 의 전 이다.)
	ID3D11Texture2D*					m_pTexture2D = nullptr;
	/* 장치에 바인딩하여 셰이더를 통해 바인딩된 이 텍스쳐에 픽셀을 기록한다.*/
	ID3D11RenderTargetView*				m_pRTV = nullptr;
	/* 이 텍스쳐를 셰이더 전역으로 전달하여 픽셀셰이더안에서 이 텍스쳐에 저장된 픽셀의 값(Diffuse, Normal, Shade)을 읽어온다. */
	ID3D11ShaderResourceView*			m_pSRV = nullptr;

private:
	_float4								m_vClearColor;		// 삭제할 색상

public:
	static CRenderTarget* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);
	virtual void Free() override;
};

END