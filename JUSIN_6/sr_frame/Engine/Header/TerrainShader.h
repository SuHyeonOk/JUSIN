#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

typedef struct tagTerrainVtx
{
	_vec3		vPos;
	_vec2		vUV;
} VTXUVTERRAIN;



class ENGINE_DLL CTerrainShader :public CVIBuffer
{
private:
	explicit CTerrainShader(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainShader(const CTerrainShader& rhs);
	virtual ~CTerrainShader()override;

public:
	virtual HRESULT Ready_Buffer(const wstring& _shaderfile, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv,
								_vec2 _uv0, _vec2 _uv1, _vec2 _uv2, _vec2 _uv3, _uint _widthcnt, _uint _heightcnt);
	virtual void Render_Buffer() override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
	static CTerrainShader* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& _shaderfile, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv,
									_vec2 _uv0, _vec2 _uv1, _vec2 _uv2, _vec2 _uv3, _uint _widthcnt, _uint _heightcnt);

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

	void Set_UVOption(_float _time, _float _speed, _float _waveheight, _float _wavefreq, _float _uvspeed)
	{
		 m_fTime += _time;
		 m_fSpeed = _speed;
		 m_fWaveHeight = _waveheight;
		 m_fWaveFreq = _wavefreq;
		 m_fUVSpeed = _uvspeed;

	}
	void Set_Env(void);


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

	LPDIRECT3DCUBETEXTURE9	gpSnowENV = NULL;

public:

	_uint m_iPlayOnFrameCnt = 0;
	_uint m_iWidthTextureCnt = 0;
	_uint m_iHeightTextureCnt = 0;

	_uint m_iFrameCnt = 0;
	_uint m_iTextureCnt_W = 0;
	_uint m_iTextureCnt_H = 0;

	_float m_fUSize = 0.f;
	_float m_fVSize = 0.f;

	_uint Mul_WidthCnt;
	_uint Mul_HeightCnt;


	_float m_fTime = 0;
	_float m_fSpeed = 0;
	_float m_fWaveHeight = 0.f;
	_float m_fWaveFreq = 0.f;
	_float m_fUVSpeed = 0.f;



	D3DXVECTOR4				gWorldLightPosition;
	D3DXVECTOR4				gLightColor;
	D3DXVECTOR4				gWorldCameraPosition;
};


END
