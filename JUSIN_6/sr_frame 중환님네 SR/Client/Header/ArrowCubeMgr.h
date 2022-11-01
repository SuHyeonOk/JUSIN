#pragma once
#include "VIBuffer.h"

#define ARROW_SIZE 300

class CArrowCubeMgr : public CVIBuffer
{
public:
	static void Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CArrowCubeMgr& GetInst()
	{
		return *s_pInst;
	}
	static void DeleteInst()
	{
		if (s_pInst != nullptr)
			s_pInst->Free();
	}

private:
	explicit CArrowCubeMgr();
	explicit CArrowCubeMgr(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrowCubeMgr() override;

	virtual HRESULT Ready_Buffer(_float fSize = 0.1f);
	virtual CComponent* Clone() override;
	virtual void Free() override;

public:
	void Add_Arrow(Engine::CTransform* pArrowTransform)
	{
		m_vecArrowTrans.push_back(pArrowTransform);
	}
	virtual void Render_Buffer() override;

private:
	static CArrowCubeMgr* s_pInst;

private:
	IDirect3DBaseTexture9* m_pTexture = nullptr;
	LPD3DXEFFECT m_pEffect = nullptr;
	LPDIRECT3DVERTEXDECLARATION9 m_pVtxDeclare = nullptr;

	LPDIRECT3DVERTEXBUFFER9		m_pVBMatrix = nullptr; // 정점 위치 보관용

	vector<Engine::CTransform*> m_vecArrowTrans;
};
