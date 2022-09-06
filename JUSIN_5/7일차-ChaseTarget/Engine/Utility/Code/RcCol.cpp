#include "RcCol.h"

USING(Engine)

CRcCol::CRcCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}


CRcCol::CRcCol(const CRcCol& rhs)
	: CVIBuffer(rhs)
{

}

CRcCol::~CRcCol()
{
}

HRESULT CRcCol::Ready_Buffer(void)
{
#pragma region ����
	/*m_dwVtxCnt = 6;
	m_dwTriCnt = 2;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwFVF = FVF_COL;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCOL*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 3���� : �迭�� ����� ù ��° ���ؽ��� �ּҸ� ����

	// ������ �� �ﰢ��
	pVertex[0].vPos = {-1.f, 1.f, 0.f};
	pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[1].vPos = { 1.f, 1.f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[2].vPos = { 1.f, -1.f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	// ���� �Ʒ� �ﰢ��
	pVertex[3].vPos = { -1.f, 1.f, 0.f };
	pVertex[3].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[4].vPos = { 1.f, -1.f, 0.f };
	pVertex[4].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[5].vPos = { -1.f, -1.f, 0.f };
	pVertex[5].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();*/
#pragma endregion ����

	m_dwVtxCnt = 516; // 256
	m_dwTriCnt = 258; // 128
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCOL*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 3���� : �迭�� ����� ù ��° ���ؽ��� �ּҸ� ����

	for (_int i = 0; i < 129; i++)
	{
		for (_int j = 0; j < 129; j++)
		{
			pVertex[i].vPos = { (float)i, (float)j, 0.f };
		}
	}

	//for (int i = 0; i < 129; i++)
	//{
	//	for (int j = 0; j < 129; j++)
	//	{
	//		pVertex[0 + j].vPos = { 0.f + (float)-i, 0.f + (float)i, 0.f };
	//		pVertex[1 + j].vPos = { 0.f + (float)i,  0.f + (float)i, 0.f };
	//		pVertex[2 + j].vPos = { 0.f + (float)i,  0.f + (float)-i, 0.f };
	//		pVertex[3 + j].vPos = { 0.f + (float)-i, 0.f + (float)-i, 0.f };
	//	}
	//}

	//// ������ �� �ﰢ��
	//pVertex[0].vPos = {-5.f, 5.f, 0.f};
	//pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//pVertex[1].vPos = { 5.f, 5.f, 0.f };
	//pVertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//pVertex[2].vPos = { 5.f, -5.f, 0.f };
	//pVertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//pVertex[3].vPos = { -5.f, -5.f, 0.f };
	//pVertex[3].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);


	m_pVB->Unlock();

	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (int i = 0; i < 129; i++)
	{
		for (int j = 0; j < 129; j++)
		{
			pIndex[j]._0 = 0 + i;
			pIndex[j]._1 = 1 + i;
			pIndex[j]._2 = 2 + i;
		}
	}
	
	//// ������ ��
	//pIndex[0]._0 = 0;
	//pIndex[0]._1 = 1;
	//pIndex[0]._2 = 2;
	//
	//// ���� �Ʒ�
	//pIndex[1]._0 = 0;
	//pIndex[1]._1 = 2;
	//pIndex[1]._2 = 3;

	m_pIB->Unlock();
		
	return S_OK;
}

void CRcCol::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CRcCol* CRcCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcCol*	pInstance = new CRcCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* Engine::CRcCol::Clone(void)
{
	return new CRcCol(*this);
}

void CRcCol::Free(void)
{
	CVIBuffer::Free();
}
