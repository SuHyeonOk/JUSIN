#include "..\..\Header\TerrainRcTex.h"

USING(Engine)

CTerrainRcTex::CTerrainRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
{
}

CTerrainRcTex::CTerrainRcTex(const CTerrainRcTex & rhs)
	:CVIBuffer(rhs)
{
}

CTerrainRcTex::~CTerrainRcTex()
{
}

HRESULT CTerrainRcTex::Ready_Buffer(const vector<_matrix>& _vecmatworld)
{
	m_iTexCnt = _vecmatworld.size();
	if (m_iTexCnt == 0)
		return E_FAIL;
	m_dwVtxCnt = (_ulong)_vecmatworld.size() * 4;
	m_dwTriCnt = (_ulong)_vecmatworld.size() * 2;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);


	VTXTEX VtxTmp[4]{};
	INDEX32 IdxTmp[2]{};

	vector<VTXTEX> vecVtxTmps;
	vector<INDEX32> vecIdxTmps;

	_vec3 vNormal{0.5f, 0.5f, 0.5f};
	D3DXVec3Normalize(&vNormal, &vNormal);
	
	for (size_t i = 0; i < _vecmatworld.size(); ++i)
	{
		VtxTmp[0].vPos = { -0.5f, 0.5f, 0.f };
		VtxTmp[0].vNormal = vNormal;
		VtxTmp[0].vTexUV = { 0.f, 0.f };

		VtxTmp[1].vPos = { 0.5f, 0.5f, 0.f };
		VtxTmp[1].vNormal = vNormal;
		VtxTmp[1].vTexUV = { 1.f, 0.f };

		VtxTmp[2].vPos = { 0.5f, -0.5f, 0.f };
		VtxTmp[2].vNormal = vNormal;
		VtxTmp[2].vTexUV = { 1.f, 1.f };

		VtxTmp[3].vPos = { -0.5f, -0.5f, 0.f };
		VtxTmp[3].vNormal = vNormal;
		VtxTmp[3].vTexUV = { 0.f, 1.f };

		for (int j = 0; j < 4; ++j)
		{
			D3DXVec3TransformCoord(&VtxTmp[j].vPos, &VtxTmp[j].vPos, &_vecmatworld[i]);
			vecVtxTmps.push_back(VtxTmp[j]);
		}

		IdxTmp[0]._0 = 0;
		IdxTmp[0]._1 = 1;
		IdxTmp[0]._2 = 2;

		IdxTmp[1]._0 = 0;
		IdxTmp[1]._1 = 2;
		IdxTmp[1]._2 = 3;

		size_t iVtxCnt = 4 * i;
		for (size_t j = 0; j < 2; ++j)
		{
			IdxTmp[j]._0 += (_ulong)iVtxCnt;
			IdxTmp[j]._1 += (_ulong)iVtxCnt;
			IdxTmp[j]._2 += (_ulong)iVtxCnt;
			vecIdxTmps.push_back(IdxTmp[j]);
		}
	}


	VTXTEX*			pVertex = nullptr;
	INDEX32*		pIndex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	memcpy(pVertex, vecVtxTmps.data(), sizeof(VTXTEX) * vecVtxTmps.size());
	m_pVB->Unlock();

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, vecIdxTmps.data(), sizeof(INDEX32) * vecIdxTmps.size());

	m_pIB->Unlock();


	return S_OK;
}

void CTerrainRcTex::ReCreateBuffer(const vector<_matrix>& _vecmatworld)
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Ready_Buffer(_vecmatworld);
}

CComponent * CTerrainRcTex::Clone(void) // dont use
{
	NULL_CHECK_RETURN(nullptr, nullptr);
	return nullptr;
}

void CTerrainRcTex::Free(void)
{
	CVIBuffer::Free();
}

CTerrainRcTex * CTerrainRcTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const vector<_matrix>& _vecmatworld)
{
	CTerrainRcTex*		pInstance = new CTerrainRcTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(_vecmatworld)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
