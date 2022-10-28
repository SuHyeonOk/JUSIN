#include "..\..\Header\RcShader.h"

// D3DXVECTOR4				gLightColor(0.7f, 0.7f, 1.0f, 1.0f);

CRcShader::CRcShader(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev), m_iPlayOnFrameCnt(0), m_iWidthTextureCnt(0), m_iHeightTextureCnt(0)
{
}

CRcShader::CRcShader(const CRcShader& rhs) : CVIBuffer(rhs), m_iPlayOnFrameCnt(0),m_iWidthTextureCnt(0),m_iHeightTextureCnt(0)
{
	m_pEffect = rhs.m_pEffect;
	m_pVtxDeclare = rhs.m_pVtxDeclare;
	UV_2 = rhs.UV_2;
	m_pEffect->AddRef();
	m_pVtxDeclare->AddRef();
}

CRcShader::~CRcShader() = default;

HRESULT CRcShader::Ready_Buffer(const wstring& _shaderfile, _vec2 _uv0, _vec2 _uv1, _vec2 _uv2, _vec2 _uv3)
{
	m_iPlayOnFrameCnt = 0;
	m_iWidthTextureCnt = 0;
	m_iHeightTextureCnt= 0;
	UV_2 = _uv2;
	ID3DXBuffer *pCompilationErrors = 0;
	DWORD dwShaderFlags = 0;
	HRESULT hr = D3DXCreateEffectFromFile(m_pGraphicDev, _shaderfile.c_str(), 0, 0,
		dwShaderFlags, 0, &m_pEffect, &pCompilationErrors);

	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	D3DVERTEXELEMENT9 VTXCUBEEX_Elements[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		// { 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		D3DDECL_END()
	};

	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexDeclaration(VTXCUBEEX_Elements, &m_pVtxDeclare), E_FAIL);

	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwVtxSize = sizeof(VTXUVPARTICLE);
	m_dwFVF = FVF_COL; // for use vtx declareation

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXUVPARTICLE*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0] = { _vec3(-1.f, 1.f, 0.f), _uv0};
	pVertex[1] = { _vec3(1.f, 1.f, 0.f), _uv1};
	pVertex[2] = { _vec3(1.f, -1.f, 0.f), _uv2};
	pVertex[3] = { _vec3(-1.f, -1.f, 0.f), _uv3};

	m_pVB->Unlock();


	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	//LoadTexture(L"T_ElectricArcs.png");

	return S_OK;
}

void CRcShader::Render_Buffer()
{

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	// m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
	m_iPlayOnFrameCnt++;

	if (m_iPlayOnFrameCnt == m_iFrameCnt)
	{
		m_iWidthTextureCnt++;
	
		if (m_iWidthTextureCnt == m_iTextureCnt_W)
		{
			m_iWidthTextureCnt = 0;
			m_iHeightTextureCnt++;
		}
	
		if (m_iHeightTextureCnt == m_iTextureCnt_H)
		{
			m_iHeightTextureCnt = 0;
		}

		m_pEffect->SetFloat("gTime", UV_2.x * m_iWidthTextureCnt);
		m_pEffect->SetFloat("gUVSpeed", UV_2.y * m_iHeightTextureCnt);
		m_iPlayOnFrameCnt = 0;
	}
	

	//Matrix
	m_pEffect->SetMatrix("gWorldMatrix", &m_matWorld);

	_matrix view, proj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
	m_pEffect->SetMatrix("gViewMatrix", &view);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &proj);
	m_pEffect->SetMatrix("gProjectionMatrix", &proj);
	//~Matrix
	m_pEffect->SetTexture("DiffuseMap_Tex", m_pTexture);
	m_pGraphicDev->SetVertexDeclaration(m_pVtxDeclare);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	

	UINT numPasses = 0;
	m_pEffect->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			m_pEffect->BeginPass(i);
			{
				m_pGraphicDev->SetIndices(m_pIB);
				m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
				//m_pEffect->CommitChanges();

			}
			m_pEffect->EndPass();
		}
	}
	m_pEffect->End();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

CComponent * CRcShader::Clone()
{
	return new CRcShader(*this);
}

void CRcShader::Free()
{
	Safe_Release(m_pEffect);
	Safe_Release(m_pVtxDeclare);
	Safe_Release(m_pTexture);

	CVIBuffer::Free();
}

CRcShader * CRcShader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& _shaderfile,_vec2 _uv0, _vec2 _uv1, _vec2 _uv2, _vec2 _uv3)
{
	CRcShader*	pInstance = new CRcShader(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(_shaderfile,_uv0,_uv1,_uv2,_uv3)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	
	

	return pInstance;
}

void CRcShader::LoadTexture(const wstring& filename)
{
	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, filename.c_str(), (LPDIRECT3DTEXTURE9*)&m_pTexture)))
	{
		
	}
}
