#include "VoxelTex.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

CVoxelTex::CVoxelTex(LPDIRECT3DDEVICE9 pGraphicDev): CVIBuffer(pGraphicDev)
{
}

CVoxelTex::CVoxelTex(const CVoxelTex& rhs): CVIBuffer(rhs)
{
}

CVoxelTex::~CVoxelTex()
{
}

_int CVoxelTex::Update_Component(const _float& fTimeDelta)
{
	return CComponent::Update_Component(fTimeDelta);
}

HRESULT CVoxelTex::Ready_Buffer(const char* strTexPath, _float fCellSize)
{
	// ... x = width, y = height, n = # 8-bit components per pixel ...
	// ... replace '0' with '1'..'4' to force that many components per pixel
	// ... but 'n' will always be the number that it would have been if you said 0
	int iWidth, iHeight, iComp;
	unsigned char* pPixel = stbi_load(strTexPath, &iWidth, &iHeight, &iComp, 0);

	if (iComp < 4)
	{
		MSG_BOX("CVoxelTex Input Tex is not png");
		return E_FAIL;
	}

	m_dwVtxCnt = 0;
	m_dwTriCnt = 0;
	m_dwVtxSize = sizeof(VertexCubeNormal2);
	m_dwFVF = FVF_CUBE_NOR2;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	// left bot : 손잡이
	// right top : 무기 끝
	//enum CubeFace {	FACE_LOOK,	FACE_BACK,	FACE_LEFT,	FACE_RIGHT,	FACE_UP, FACE_DOWN,	FACE_END};

	_float w2 = 0.5f;
	_float h2 = 0.5f;
	_float d2 = 0.5f;

	VertexCubeNormal2 tVtxCubeDefault[FACE_END][4]
	{
		{
			{{-w2, -h2, +d2}, {0.f, 0.f, 1.f}, {}},
			{{+w2, -h2, +d2}, {0.f, 0.f, 1.f}, {}},
			{{+w2, +h2, +d2}, {0.f, 0.f, 1.f}, {}},
			{{-w2, +h2, +d2}, {0.f, 0.f, 1.f}, {}}
		}, // look
		{
			{{-w2, -h2, -d2}, {0.f, 0.f, -1.f}, {}},
			{{-w2, +h2, -d2}, {0.f, 0.f, -1.f}, {}},
			{{+w2, +h2, -d2}, {0.f, 0.f, -1.f}, {}},
			{{+w2, -h2, -d2}, {0.f, 0.f, -1.f}, {}}
		}, // back
		{
			{{-w2, -h2, +d2}, {-1.f, 0.f, 0.f}, {}},
			{{-w2, +h2, +d2}, {-1.f, 0.f, 0.f}, {}},
			{{-w2, +h2, -d2}, {-1.f, 0.f, 0.f}, {}},
			{{-w2, -h2, -d2}, {-1.f, 0.f, 0.f}, {}}
		}, // left
		{
			{{+w2, -h2, -d2}, {1.f, 0.f, 0.f}, {}},
			{{+w2, +h2, -d2}, {1.f, 0.f, 0.f}, {}},
			{{+w2, +h2, +d2}, {1.f, 0.f, 0.f}, {}},
			{{+w2, -h2, +d2}, {1.f, 0.f, 0.f}, {}}
		}, // right
		{
			{{-w2, +h2, -d2}, {0.f, 1.f, 0.f}, {}},
			{{-w2, +h2, +d2}, {0.f, 1.f, 0.f}, {}},
			{{+w2, +h2, +d2}, {0.f, 1.f, 0.f}, {}},
			{{+w2, +h2, -d2}, {0.f, 1.f, 0.f}, {}}
		}, // up
		{
			{{-w2, -h2, -d2}, {0.f, -1.f, 0.f}, {}},
			{{+w2, -h2, -d2}, {0.f, -1.f, 0.f}, {}},
			{{+w2, -h2, +d2}, {0.f, -1.f, 0.f}, {}},
			{{-w2, -h2, +d2}, {0.f, -1.f, 0.f}, {}}
		}, // down
	};

	// resize
	for (int i = 0; i < FACE_END; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			tVtxCubeDefault[i][j].vPos.x *= fCellSize;
			tVtxCubeDefault[i][j].vPos.y *= fCellSize;
			tVtxCubeDefault[i][j].vPos.z *= fCellSize;
		}
	}

	vector<VertexCubeNormal2> vecVtxCube2;
	vector<INDEX32> vecIdx;

	for (int i = 0; i < iHeight; ++i) // x 축
	{
		for (int j = 0; j < iWidth; ++j) // z 축
		{
			const int iIdx = iComp * (i * iWidth + j);
			const int a = pPixel[iIdx + 3];
			if (a == 0) continue;

			// const int iReversedHeight = iHeight - j;

			_vec2 vUV[4]
			{
				{(float)j / iHeight, (float)i / iWidth},
				{(float)j / iHeight, (float)(i + 1) / iWidth},
				{(float)(j + 1) / iHeight, (float)(i + 1) / iWidth},
				{(float)(j + 1) / iHeight, (float)i / iWidth}
			};

			VertexCubeNormal2 tVtxTmp[FACE_END][4];
			::memcpy(tVtxTmp, tVtxCubeDefault, sizeof(tVtxTmp));

			for (int w = 0; w < FACE_END; ++w)
			{
				for (size_t k = 0; k < 4; ++k)
				{
					tVtxTmp[w][k].vPos.x += (_float)i * fCellSize;
					tVtxTmp[w][k].vPos.z += (_float)j * fCellSize;
					tVtxTmp[w][k].vTexUV = vUV[k];
					vecVtxCube2.push_back(tVtxTmp[w][k]);
				}
				vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 1, m_dwVtxCnt + 2});
				vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 2, m_dwVtxCnt + 3});
				m_dwVtxCnt += 4;
				m_dwTriCnt += 2;
			}
		}
	}


	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VertexCubeNormal2* pVertex = nullptr;
	INDEX32* pIndex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	memcpy(pVertex, vecVtxCube2.data(), sizeof(VertexCubeNormal2) * vecVtxCube2.size());
	m_pVB->Unlock();

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, vecIdx.data(), sizeof(INDEX32) * vecIdx.size());
	m_pIB->Unlock();

	stbi_image_free(pPixel);

	return S_OK;
}

void CVoxelTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CVoxelTex* CVoxelTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const char* strTexPath, _float fCellSize)
{
	CVoxelTex* pInstance = new CVoxelTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(strTexPath, fCellSize)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* CVoxelTex::Clone()
{
	return new CVoxelTex(*this);
}
