#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainRcTex :
	public CVIBuffer
{
public:
	explicit  CTerrainRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainRcTex(const CTerrainRcTex& rhs);
	virtual ~CTerrainRcTex();

public:
	virtual	 HRESULT	Ready_Buffer(const vector<_matrix>& _vecmatworld);
	void ReCreateBuffer(const vector<_matrix>& _vecmatworld);

	static CTerrainRcTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const vector<_matrix>& _vecmatworld);
	virtual CComponent*	Clone(void) override;
	virtual void Free(void) override;

	size_t GetTexCnt() const { return m_iTexCnt; }

private:
	size_t m_iTexCnt = 0;
};

END
