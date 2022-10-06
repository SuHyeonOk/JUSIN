#include "TriCol.h"

USING(Engine)

CTriCol::CTriCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}


CTriCol::CTriCol(const CTriCol& rhs)
	: CVIBuffer(rhs)
{

}

CTriCol::~CTriCol()
{
}

HRESULT CTriCol::Ready_Buffer(void)
{

	// 버텍스 버퍼
	m_dwVtxCnt = 3;
	m_dwTriCnt = 1;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCOL*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0); // Lock : 버텍스 버퍼 접근권한을 막는다.
	// 3인자 : 배열에 저장된 첫 번째 버텍스의 주소를 얻어옴

	// 버텍스 버퍼는 그릴때마다 새로갱신되어야 함.
	// 정점좌표(로컬)을 투영좌표로 잡아버리면, 그 이후 항등항렬을 곱하여 I I I
	// 투영좌표를 얻은 것과 같다.

	pVertex[0].vPos = { 0.f, 1.f, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[1].vPos = { 1.f, -1.f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[2].vPos = { -1.f, -1.f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);

	m_pVB->Unlock();

	// 인덱스 버퍼
	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();
	return S_OK;
}

void CTriCol::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CTriCol* CTriCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTriCol*	pInstance = new CTriCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent * CTriCol::Clone(void)
{
	return new CTriCol(*this); // 복사생성자 => 얕은 복사
}

void CTriCol::Free(void)
{
	CVIBuffer::Free();
}
