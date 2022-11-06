#include "..\..\Header\CubeTex.h"
#include "GameUtilMgr.h"

USING(Engine)

CCubeTex::CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}


Engine::CCubeTex::CCubeTex(const CCubeTex& rhs)
	: CVIBuffer(rhs)
{

}

CCubeTex::~CCubeTex()
{
}

HRESULT CCubeTex::Ready_Buffer(void)
{
	m_dwVtxCnt = 0;
	m_dwTriCnt = 0;

	vector<VertexCubeNormal> vecVtx;
	vector<INDEX32> vecIdx;

	VertexCubeNormal vtx;


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
			vecVtx.push_back(vtx);
		}
		vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 1, m_dwVtxCnt + 2});
		vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 2, m_dwVtxCnt + 3});
		m_dwVtxCnt += 4;
		m_dwTriCnt += 2;
	}

	m_dwVtxSize = sizeof(VertexCubeNormal);
	m_dwFVF = FVF_CUBE_NOR;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VertexCubeNormal*		pVertex = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	memcpy(pVertex, vecVtx.data(), sizeof(VertexCubeNormal) * vecVtx.size());
	m_pVB->Unlock();

	INDEX32*		pIndex = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, vecIdx.data(), sizeof(INDEX32) * vecIdx.size());
	m_pIB->Unlock();

	return S_OK;
}

void CCubeTex::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CCubeTex * CCubeTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCubeTex*	pInstance = new CCubeTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent * CCubeTex::Clone(void)
{
	return new CCubeTex(*this);
}

void CCubeTex::Free(void)
{
	CVIBuffer::Free();
}
