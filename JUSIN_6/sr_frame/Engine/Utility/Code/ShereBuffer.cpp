#include "..\..\Header\ShereBuffer.h"

constexpr float XM_PI        = 3.141592654f;
constexpr float XM_2PI       = 6.283185307f;

CShereBuffer::CShereBuffer(LPDIRECT3DDEVICE9 pGraphicDev) : CVIBuffer(pGraphicDev)
{
}

CShereBuffer::CShereBuffer(const CShereBuffer& rhs) : CVIBuffer()
{
	m_pGraphicDev = rhs.m_pGraphicDev;
	m_pGraphicDev->AddRef();

	gpSphere = rhs.gpSphere;
	gpSphere->AddRef();
}

CShereBuffer::~CShereBuffer()
{
}

HRESULT CShereBuffer::Ready_Buffer(_uint iStackCount, _uint iSliceCount)
{
	if (FAILED(D3DXLoadMeshFromX(L"../Bin/Resource/Shader/Sphere.x", D3DXMESH_SYSTEMMEM, m_pGraphicDev, NULL, NULL, NULL, NULL, &gpSphere)))
	{
		OutputDebugString(L"∏µ® ∑Œµ˘ Ω«∆–: ");
		OutputDebugString(L"../Bin/Resource/Shader/Sphere.x");
		OutputDebugString(L"\n");
		return E_FAIL;
	};

	return S_OK;
	vector<VTXCUBE2> vecVtx;

	VTXCUBE2 v;
	_float radius = 0.5f;
	v.vPos = _vec3(0.0f, 0.5f, 0.0f);
	v.vTexUV = _vec2(0.5f, 0.0f);
	vecVtx.push_back(v);

	float stackAngle = XM_PI / iStackCount;
	float sliceAngle = XM_2PI / iSliceCount;

	float deltaU = 1.f / static_cast<float>(iSliceCount);
	float deltaV = 1.f / static_cast<float>(iStackCount);

	// ∞Ì∏Æ∏∂¥Ÿ µπ∏Èº≠ ¡§¡°¿ª ∞ËªÍ«—¥Ÿ (∫œ±ÿ/≥≤±ÿ ¥‹¿œ¡°¿∫ ∞Ì∏Æ∞° X)
	for (_uint y = 1; y <= iStackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// ∞Ì∏Æø° ¿ßƒ°«— ¡§¡°
		for (_uint x = 0; x <= iSliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.vPos.x = radius * sinf(phi) * cosf(theta);
			v.vPos.y = radius * cosf(phi);
			v.vPos.z = radius * sinf(phi) * sinf(theta);

			v.vTexUV = _vec2(deltaU * x, deltaV * y);

			// v.normal = v.vPos;
			// v.normal.Normalize();
			//
			// v.tangent.x = -radius * sinf(phi) * sinf(theta);
			// v.tangent.y = 0.0f;
			// v.tangent.z = radius * sinf(phi) * cosf(theta);
			// v.tangent.Normalize();

			vecVtx.push_back(v);
		}
	}

	// ≥≤±ÿ
	v.vPos = _vec3(0.0f, -radius, 0.0f);
	v.vTexUV = _vec2(0.5f, 1.0f);
	vecVtx.push_back(v);

	vector<INDEX32> vecIdx;
	// vector<_uint> idx(36);
	INDEX32 idx;
	// ∫œ±ÿ ¿Œµ¶Ω∫
	for (_uint i = 0; i <= iSliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx._0 = 0;
		idx._1 = i + 2;
		idx._2 = i + 1;
		vecIdx.push_back(idx);
	}

	// ∏ˆ≈Î ¿Œµ¶Ω∫
	_uint ringVertexCount = iSliceCount + 1;
	for (_uint y = 0; y < iStackCount - 2; ++y)
	{
		for (_uint x = 0; x < iSliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx._0 = 1 + (y) * ringVertexCount + (x);
			idx._1 = 1 + (y) * ringVertexCount + (x + 1);
			idx._2 = 1 + (y + 1) * ringVertexCount + (x);
			vecIdx.push_back(idx);
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx._0 = 1 + (y + 1) * ringVertexCount + (x);
			idx._1 = 1 + (y) * ringVertexCount + (x + 1);
			idx._2 = 1 + (y + 1) * ringVertexCount + (x + 1);
			vecIdx.push_back(idx);
		}
	}

	// ≥≤±ÿ ¿Œµ¶Ω∫
	_uint bottomIndex = static_cast<_uint>(vecVtx.size()) - 1;
	_uint lastRingStartIndex = bottomIndex - ringVertexCount;
	for (_uint i = 0; i < iSliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx._0 = bottomIndex;
		idx._1 = lastRingStartIndex + i;
		idx._2 = lastRingStartIndex + i + 1;
		vecIdx.push_back(idx);
	}

	m_dwVtxCnt = vecVtx.size();
	m_dwTriCnt = vecIdx.size();
	m_dwVtxSize = sizeof(VTXCUBE2);
	m_dwFVF = FVF_CUBE2;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCUBE2* pVertex = nullptr;
	INDEX32* pIndex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	memcpy(pVertex, vecVtx.data(), sizeof(VTXCUBE2) * vecVtx.size());
	m_pVB->Unlock();

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	memcpy(pIndex, vecIdx.data(), sizeof(INDEX32) * vecIdx.size());
	m_pIB->Unlock();
}

void CShereBuffer::Render_Buffer()
{
	gpSphere->DrawSubset(0);
}

CShereBuffer* CShereBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iiStackCount, _uint iiSliceCount)
{
	CShereBuffer*	pInstance = new CShereBuffer(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(iiStackCount, iiSliceCount)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* CShereBuffer::Clone()
{
	return new CShereBuffer(*this);
}

void CShereBuffer::Free()
{
	Safe_Release(gpSphere);
	Safe_Release(m_pGraphicDev);
}
