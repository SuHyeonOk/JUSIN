#include "RcTex.h"
#include "GameUtilMgr.h"

CRcTex::CRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRcTex::CRcTex(const CVIBuffer& rhs)
	: CVIBuffer(rhs)
{
}

CRcTex::~CRcTex() = default;

HRESULT CRcTex::Ready_Buffer()
{
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 3인자 : 배열에 저장된 첫 번째 버텍스의 주소를 얻어옴

	// 오른쪽 위 삼각형
	pVertex[0].vPos = { -0.5f, 0.5f, 0.f };
	pVertex[0].vTexUV = {0.f, 0.f};

	pVertex[1].vPos = { 0.5f, 0.5f, 0.f };
	pVertex[1].vTexUV = {1.f, 0.f};

	pVertex[2].vPos = { 0.5f, -0.5f, 0.f };
	pVertex[2].vTexUV = {1.f, 1.f};

	pVertex[3].vPos = { -0.5f, -0.5f, 0.f };
	pVertex[3].vTexUV = {0.f, 1.f};

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

	return S_OK;
}

void CRcTex::Render_Buffer()
{
	if (false == CGameUtilMgr::FloatCmp(m_fPreProfress, m_fProgress))
	{
		VTXTEX*		pVertex = nullptr;
		m_pVB->Lock(0, 0, (void**)&pVertex, 0);
		pVertex[0].vPos = { -0.5f, m_fProgress - 0.5f , 0.f };
		pVertex[0].vTexUV = { 0.f, 1.f - m_fProgress };

		pVertex[1].vPos = { 0.5f, m_fProgress - 0.5f, 0.f };
		pVertex[1].vTexUV = { 1.f, 1.f - m_fProgress };

		pVertex[2].vPos = { 0.5f, -0.5f, 0.f };
		pVertex[2].vTexUV = { 1.f, 1.f };

		pVertex[3].vPos = { -0.5f, -0.5f, 0.f };
		pVertex[3].vTexUV = { 0.f, 1.f };
		m_pVB->Unlock();

		m_fPreProfress = m_fProgress;
	}

	CVIBuffer::Render_Buffer();
}

CRcTex* CRcTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcTex*	pInstance = new CRcTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* CRcTex::Clone()
{
	return new CRcTex(*this);
}

void CRcTex::Free()
{
	CVIBuffer::Free();
}
