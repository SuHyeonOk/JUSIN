#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
private:
	explicit CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex();

public:
	HRESULT Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	virtual void Render_Buffer(void) override;

private:	// CTerrainTex
	HANDLE				m_hFile; // 파일입출력을 위해

	BITMAPFILEHEADER	m_fH;	// 파일 헤더
	BITMAPINFOHEADER	m_iH;	// 정보 헤더

public:
	static CTerrainTex*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	CComponent*	Clone(void);
	virtual void Free(void) override;
};

END