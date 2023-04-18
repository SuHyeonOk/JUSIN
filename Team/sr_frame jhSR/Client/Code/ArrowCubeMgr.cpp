#include "stdafx.h"
#include "ArrowCubeMgr.h"
#include "GameUtilMgr.h"

CArrowCubeMgr* CArrowCubeMgr::s_pInst = nullptr;

D3DVERTEXELEMENT9 BoxVTXInstance_Elements2[] =
{
    { 0, 0,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
    { 0, 12, D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
    { 1, 0,     D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT,     0 },
    { 1, 16,     D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT,     1 },
    { 1, 32,     D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT,     2 },
    { 1, 48,     D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT,     3 },
    D3DDECL_END()
};

CArrowCubeMgr::CArrowCubeMgr() : CVIBuffer(nullptr)
{
	
}

CArrowCubeMgr::CArrowCubeMgr(LPDIRECT3DDEVICE9 pGraphicDev): CVIBuffer(pGraphicDev)
{
}

CArrowCubeMgr::~CArrowCubeMgr()
{
}

HRESULT CArrowCubeMgr::Ready_Buffer(float fSize)
{
	ID3DXBuffer* pCompilationErrors = 0;
	DWORD dwShaderFlags = 0;

	// m_vecArrowWorld.resize(10);
	//
	// for(int i = 0; i< m_vecArrowWorld.size(); ++i)
	// 	CGameUtilMgr::MatWorldComposeEuler(m_vecArrowWorld[i], {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f}, {_float(i), 0.f, 0.f});

#if _DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	D3DXCreateEffectFromFile(m_pGraphicDev,
	                         L"../Bin/Resource/Shader/Instancing_arrow.fx", 0, 0, dwShaderFlags, 0, &m_pEffect,
	                         &pCompilationErrors);

	if (!m_pEffect && pCompilationErrors)
	{
		int size = pCompilationErrors->GetBufferSize();
		void* ack = pCompilationErrors->GetBufferPointer();

		if (ack)
		{
			char* str = new char[size];
			sprintf(str, (const char*)ack, size);
			OutputDebugStringA(str);
			_CRASH(str);
			delete[] str;
		}
	}

	FAILED_CHECK_RETURN(
		D3DXCreateTextureFromFile(m_pGraphicDev, L"../Bin/Resource/Texture/arrow/arrow_color.png", (LPDIRECT3DTEXTURE9*)
			&m_pTexture), E_FAIL);
		FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexDeclaration(BoxVTXInstance_Elements2, &m_pVtxDeclare), E_FAIL);

	m_dwVtxCnt = 0;
	m_dwTriCnt = 0;
	m_dwVtxSize = sizeof(tagVertexCubeTex2);
	m_dwFVF = 0;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	vector<tagVertexCubeTex2> vecVTXInst;
	vector<INDEX32> vecIdx;

	static _vec2 vBodyUV[4]
	{
		{0.f, 0.f}, {1.f / 3.f, 0.f}, {0.f, 1.f}, {1.f / 3.f, 1.f}
	};
	static _vec2 vHeadUV[4]
	{
		{1.f / 3.f, 0.f}, {2.f / 3.f, 0.f}, {1.f / 3.f, 1.f}, {2.f / 3.f, 1.f}
	};
	static _vec2 vTailUV[4]
	{
		{2.f / 3.f, 0.f}, {1.f, 0.f}, {2.f / 3.f, 1.f}, {1.f, 1.f}
	};

	tagVertexCubeTex2 DefaultCubeVtx[FACE_END][4]{};
	tagVertexCubeTex2 ArrowVtx[5][FACE_END][4]{};
	_matrix matTrans;

	// make arrow vtx
	{
		{
			// arrow head point
			CGameUtilMgr::MatWorldComposeEuler(matTrans, {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 5.5f});
			for (int i = 0; i < FACE_END; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					D3DXVec3TransformCoord(&DefaultCubeVtx[i][j].vPos, &CGameUtilMgr::s_vFaceCubeVtx[i][j], &matTrans);
					DefaultCubeVtx[i][j].vTexUV = vHeadUV[j];
					ArrowVtx[0][i][j] = DefaultCubeVtx[i][j];
				}
			}
		}
		{
			// arrow head
			CGameUtilMgr::MatWorldComposeEuler(matTrans, {1.f, 3.f, 1.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 4.5f});
			for (int i = 0; i < FACE_END; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					D3DXVec3TransformCoord(&DefaultCubeVtx[i][j].vPos, &CGameUtilMgr::s_vFaceCubeVtx[i][j], &matTrans);
					DefaultCubeVtx[i][j].vTexUV = vHeadUV[j];
					ArrowVtx[1][i][j] = DefaultCubeVtx[i][j];
				}
			}
		}
		{
			//arrow body
			CGameUtilMgr::MatWorldComposeEuler(matTrans, {1.f, 1.f, 7.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.5f});
			for (int i = 0; i < FACE_END; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					D3DXVec3TransformCoord(&DefaultCubeVtx[i][j].vPos, &CGameUtilMgr::s_vFaceCubeVtx[i][j], &matTrans);
					DefaultCubeVtx[i][j].vTexUV = vBodyUV[j];
					ArrowVtx[2][i][j] = DefaultCubeVtx[i][j];
				}
			}
		}
		{
			//arrow tail left
			CGameUtilMgr::MatWorldComposeEuler(matTrans, {1.f, 1.f, 2.f}, {0.f, 0.f, 0.f}, {1.f, 0.f, -3.f});
			for (int i = 0; i < FACE_END; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					D3DXVec3TransformCoord(&DefaultCubeVtx[i][j].vPos, &CGameUtilMgr::s_vFaceCubeVtx[i][j], &matTrans);
					DefaultCubeVtx[i][j].vTexUV = vTailUV[j];
					ArrowVtx[3][i][j] = DefaultCubeVtx[i][j];
				}
			}
		}
		{
			//arrow tail 
			CGameUtilMgr::MatWorldComposeEuler(matTrans, {1.f, 1.f, 2.f}, {0.f, 0.f, 0.f}, {-1.f, 0.f, -3.f});
			for (int i = 0; i < FACE_END; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					D3DXVec3TransformCoord(&DefaultCubeVtx[i][j].vPos, &CGameUtilMgr::s_vFaceCubeVtx[i][j], &matTrans);
					DefaultCubeVtx[i][j].vTexUV = vTailUV[j];
					ArrowVtx[4][i][j] = DefaultCubeVtx[i][j];
				}
			}
		}
		//resize
		CGameUtilMgr::MatWorldComposeEuler(matTrans, {fSize, fSize, fSize}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f});
		for (int k = 0; k < 5; ++k)
		{
			for (int i = 0; i < FACE_END; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					D3DXVec3TransformNormal(&ArrowVtx[k][i][j].vPos, &ArrowVtx[k][i][j].vPos, &matTrans);
				}
			}
		}
	}

	for (int k = 0; k < 5; ++k) // arrow의 각 큐브 부품
	{
		for (int i = 0; i < FACE_END; ++i) // 각 큐브 면
		{
			for (int j = 0; j < 4; ++j) // 면에 있는 버텍스
			{
				vecVTXInst.push_back(ArrowVtx[k][i][j]);
			}
			vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 1, m_dwVtxCnt + 2});
			vecIdx.push_back({m_dwVtxCnt, m_dwVtxCnt + 2, m_dwVtxCnt + 3});
			m_dwVtxCnt += 4;
			m_dwTriCnt += 2;
		}
	}

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	tagVertexCubeTex2* pVertex = nullptr;
	INDEX32* pIndex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	memcpy(pVertex, vecVTXInst.data(), sizeof(tagVertexCubeTex2) * vecVTXInst.size());
	m_pVB->Unlock();

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, vecIdx.data(), sizeof(INDEX32) * vecIdx.size());
	m_pIB->Unlock();

	m_pGraphicDev->CreateVertexBuffer( ARROW_SIZE * sizeof(_matrix), 0, 0,
                                              D3DPOOL_MANAGED, &m_pVBMatrix, 0 );
	return S_OK;
}

void CArrowCubeMgr::Render_Buffer()
{
	if (m_vecArrowTrans.empty()) return;

	const size_t iMaxRenderCnt = m_vecArrowTrans.size() > ARROW_SIZE ? ARROW_SIZE : m_vecArrowTrans.size();

	_matrix* pIPos;
	m_pVBMatrix->Lock(0, NULL, ( void** )&pIPos, 0);
	ZeroMemory(pIPos, sizeof(_matrix) * ARROW_SIZE);
	for (size_t i = 0; i < iMaxRenderCnt; ++i)
		pIPos[i] = m_vecArrowTrans[i]->m_matWorld;
	m_pVBMatrix->Unlock();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	_matrix view, proj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &proj);
	m_pEffect->SetTexture("g_txScene", m_pTexture);
	m_pEffect->SetMatrix("g_mView", &view);
	m_pEffect->SetMatrix("g_mProj", &proj);

	m_pGraphicDev->SetVertexDeclaration(m_pVtxDeclare);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | ARROW_SIZE );

	m_pGraphicDev->SetStreamSource( 1, m_pVBMatrix, 0, sizeof(_matrix));
	m_pGraphicDev->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);

	m_pGraphicDev->SetIndices(m_pIB);

	UINT numPasses = 0;
	m_pEffect->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			m_pEffect->BeginPass(i);
			{
				m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
			}
			m_pEffect->EndPass();
		}
	}
	m_pEffect->End();

	m_pGraphicDev->SetStreamSourceFreq( 0, 1 );
	m_pGraphicDev->SetStreamSourceFreq( 1, 1 );

	m_vecArrowTrans.clear();
}

CComponent* CArrowCubeMgr::Clone()
{
	_CRASH("Dont clone this");
	return nullptr;
}

void CArrowCubeMgr::Free()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pEffect);
	Safe_Release(m_pVtxDeclare);
	Safe_Release(m_pVBMatrix);

	CVIBuffer::Free();

	delete s_pInst;
	s_pInst = nullptr;
}

void CArrowCubeMgr::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (s_pInst == nullptr)
	{
		s_pInst = new CArrowCubeMgr(pGraphicDev);
		if (FAILED(s_pInst->Ready_Buffer()))
		{
			Safe_Release(s_pInst);
			_CRASH("Fail cube arrow mgr");
		}
	}
}
