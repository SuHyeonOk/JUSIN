#include "TerrainCubeTex.h"

#include "GameUtilMgr.h"

USING(Engine)

CTerrainCubeTex::CTerrainCubeTex(LPDIRECT3DDEVICE9 pGraphicDev) :CVIBuffer(pGraphicDev)
{
}

CTerrainCubeTex::CTerrainCubeTex(const CTerrainCubeTex & rhs)
	: CVIBuffer(rhs)
{
}

CTerrainCubeTex::~CTerrainCubeTex()
{
}

HRESULT CTerrainCubeTex::Ready_Buffer(const vector<_matrix>& _vecmatworld)
{
	m_iCubeCnt = _vecmatworld.size();
	if (m_iCubeCnt == 0)
		return E_FAIL;

	m_dwVtxCnt = 0;
	m_dwTriCnt = 0;
	m_dwVtxSize = sizeof(VertexCubeNormal);
	m_dwFVF = FVF_CUBE_NOR;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;


	vector<VertexCubeNormal> vecVtx;
	vector<INDEX32> vecIdx;

	VertexCubeNormal vtx;

	for (auto& cubeWorld : _vecmatworld)
	{
		vector<VertexCubeNormal> vecTmp;
		for (int i = 0; i < FACE_END; ++i)
		{
			for (size_t j = 0; j < 4; ++j)
			{
				vtx.vPos = CGameUtilMgr::s_vFaceCubeVtx[i][j];
				vtx.vTexUV = CGameUtilMgr::s_vFaceCubeVtx[i][j];
				switch (CubeFace(i))
				{
					case FACE_LOOK:
						vtx.vNormal = {0.f, 0.f, 1.f};
						break;
					case FACE_BACK:
						vtx.vNormal = {0.f, 0.f, -1.f};
						break;
					case FACE_LEFT:
						vtx.vNormal = {-1.f, 0.f, 0.f};
						break;
					case FACE_RIGHT:
						vtx.vNormal = {1.f, 0.f, 0.f};
						break;
					case FACE_UP:
						vtx.vNormal = {0.f, 1.f, 0.f};
						break;
					case FACE_DOWN:
						vtx.vNormal = {0.f, -1.f, 0.f};
						break;
					case FACE_END: 
						break;
				default: ;
				}
				vecTmp.push_back(vtx);
			}
			vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 1, m_dwVtxCnt + 2});
			vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 2, m_dwVtxCnt + 3});
			m_dwVtxCnt += 4;
			m_dwTriCnt += 2;
		}
			
		for (size_t k = 0; k < vecTmp.size(); ++k)
		{
			D3DXVec3TransformCoord(&vecTmp[k].vPos, &vecTmp[k].vPos, &cubeWorld);
			vecVtx.push_back(vecTmp[k]);
		}
	}

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);


	VertexCubeNormal*		pVertex = nullptr;
	INDEX32*		pIndex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	memcpy(pVertex, vecVtx.data(), sizeof(VertexCubeNormal) * vecVtx.size());
	m_pVB->Unlock();

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, vecIdx.data(), sizeof(INDEX32) * vecIdx.size());

	m_pIB->Unlock();

	return S_OK;
}

void CTerrainCubeTex::ReCreateBuffer(const vector<_matrix>& _vecmatworld)
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Ready_Buffer(_vecmatworld);
}

CComponent * CTerrainCubeTex::Clone(void) // dont use
{
	NULL_CHECK_RETURN(nullptr, nullptr);
	return nullptr;
}

CTerrainCubeTex * CTerrainCubeTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const vector<_matrix>& _vecmatworld)
{
	CTerrainCubeTex* pInstance = new CTerrainCubeTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(_vecmatworld)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTerrainCubeTex::Free(void)
{
	CVIBuffer::Free();
}
