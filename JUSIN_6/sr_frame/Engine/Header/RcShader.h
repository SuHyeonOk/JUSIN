#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

typedef struct tagParticleVtx
{
	_vec3		vPos;
	_vec2		vUV;
	D3DXCOLOR	Color;
} VTXUVPARTICLE;


class ENGINE_DLL CRcShader :public CVIBuffer
{
private:
	explicit CRcShader(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcShader(const CRcShader& rhs);
	virtual ~CRcShader() override;

public:
	virtual HRESULT Ready_Buffer(const wstring& _shaderfile, _vec2 _uv0, _vec2 _uv1, _vec2 _uv2, _vec2 _uv3);
	virtual void Render_Buffer() override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
	static CRcShader* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& _shaderfile, _vec2 _uv0, _vec2 _uv1, _vec2 _uv2, _vec2 _uv3);


public:
	void Set_Texture(IDirect3DBaseTexture9* _ptexture)
	{
		m_pTexture = _ptexture;
		m_pTexture->AddRef();
	}

	void Set_TextureOption(_uint _framecnt, _uint _texturecntw, _uint _texturecnth)
	{
		m_iFrameCnt = _framecnt;
		m_iTextureCnt_W = _texturecntw;
		m_iTextureCnt_H = _texturecnth;
	}

	_bool Check_TextureCnt(_uint _widthtexturecnt, _uint heighttexturecnt)
	{
		if (m_iWidthTextureCnt == _widthtexturecnt && m_iHeightTextureCnt == heighttexturecnt)
			return true;
		else
			return false;
	}

	void Check_Alpha(_bool _TrueIsAlphaOn)
	{
		m_bIsAlpha = _TrueIsAlphaOn;
	}

	const _uint& Get_WidthTextureCnt(void)
	{
		return m_iWidthTextureCnt;
	}
	const _uint& Get_HeightTextureCnt(void)
	{
		return m_iHeightTextureCnt;
	}

	_matrix					 m_matWorld;
	_matrix					 m_ViewMatrix;
	_matrix					 m_PrjMatrix;
	_vec4					 m_WorldCamPos;
	_vec4					 m_WorldLightPos{ 100.f, 0.f, 100.f, 1.f };

public:
	void LoadTexture(const wstring& filename);
private:
	LPD3DXEFFECT					m_pEffect = nullptr;
	LPDIRECT3DVERTEXDECLARATION9	m_pVtxDeclare = nullptr;
	IDirect3DBaseTexture9*			m_pTexture = nullptr;


private:
	_bool m_bIsAlpha = false;
	_uint m_iPlayOnFrameCnt = 0;
	_uint m_iWidthTextureCnt = 0;
	_uint m_iHeightTextureCnt= 0;

	_uint m_iFrameCnt = 0;
	_uint m_iTextureCnt_W = 0;
	_uint m_iTextureCnt_H= 0;

	_float m_fUSize = 0.f;
	_float m_fVSize = 0.f;

	_vec2 UV_0;
	_vec2 UV_1;
	_vec2 UV_2;
	_vec2 UV_3;



};

END