#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CSphereMesh :	public CVIBuffer
{
private:
	explicit CSphereMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSphereMesh(const CSphereMesh& rhs);
	virtual ~CSphereMesh() override;

public:
	virtual CComponent* Clone() override;
	virtual HRESULT Ready_Buffer(const wstring& _shaderfile, const wstring& _texturefile);
	virtual void Render_Buffer() override;
	virtual void Free() override;

	HRESULT LoadAssets(const wstring& _shaderfile, const wstring& _texturefile);


	LPDIRECT3DTEXTURE9	LoadTexture(LPCWSTR filename);
	LPD3DXEFFECT		LoadShader(LPCWSTR filename);
	LPD3DXMESH			LoadModel(LPCWSTR filename);

	static CSphereMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& _shaderfile, const wstring& _texturefile);



	void Set_Texture(IDirect3DBaseTexture9* _ptexture)
	{
		// m_pTexture = _ptexture;
		// m_pTexture->AddRef();
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

public:
	_matrix	m_matWorld;

private:
	// LPD3DXEFFECT					m_pEffect = nullptr;
	// LPDIRECT3DVERTEXDECLARATION9	m_pVtxDeclare = nullptr;

	float					gRotationY = 0.0f;


	LPD3DXMESH				gpSphere = NULL;

	LPD3DXEFFECT			gpTextureMappingShader = NULL;

	LPDIRECT3DTEXTURE9		gpEarthDM = NULL;


	//IDirect3DBaseTexture9*			m_pTexture = nullptr;

private:
	_uint m_iPlayOnFrameCnt = 0;
	_uint m_iWidthTextureCnt = 0;
	_uint m_iHeightTextureCnt = 0;

	_uint m_iFrameCnt = 0;
	_uint m_iTextureCnt_W = 0;
	_uint m_iTextureCnt_H = 0;

	_float m_fUSize = 0.f;
	_float m_fVSize = 0.f;

	_vec2 UV_0;
	_vec2 UV_1;
	_vec2 UV_2;
	_vec2 UV_3;
};

END