#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainCubeTex : public CVIBuffer
{
protected:
	explicit CTerrainCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainCubeTex(const CTerrainCubeTex& rhs);
	virtual ~CTerrainCubeTex();

public:
	virtual HRESULT		Ready_Buffer(const vector<_matrix>& _vecmatworld);
	void ReCreateBuffer(const vector<_matrix>& _vecmatworld);
	virtual CComponent*	Clone(void) override;
	virtual void Free(void) override;

	static CTerrainCubeTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const vector<_matrix>& _vecmatworld);

	size_t GetCubeCnt() const { return m_iCubeCnt; }
	
private:
	size_t m_iCubeCnt = 0;
	
};

END