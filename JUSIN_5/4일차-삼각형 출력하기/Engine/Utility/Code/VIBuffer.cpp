#include "VIBuffer.h"

USING(Engine)

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_dwVtxCnt(0)
	, m_dwVtxSize(0)
	, m_dwFVF(0)
	, m_dwTriCnt(0)
{
	m_pGraphicDev->AddRef();
}

Engine::CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_dwVtxCnt(rhs.m_dwVtxCnt)
	, m_dwVtxSize(rhs.m_dwVtxSize)
	, m_dwFVF(rhs.m_dwFVF)
	, m_dwTriCnt(rhs.m_dwTriCnt)
{
	m_pGraphicDev->AddRef();

	m_pVB->AddRef();
	//m_pIB->AddRef();
}

CVIBuffer::~CVIBuffer()
{
}

HRESULT CVIBuffer::Ready_Buffer(void)
{
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize,  // �׸����� �ϴ� ������ �޸� ũ��
												0,							// �޸� ���� ����� �ɼ�, 0�� ��� ���� ���۸� ���(D3DUSAGE_DYNAMIC�� ������ ��� ���� ���� �뵵�� ����)
												m_dwFVF,					// ������ �Ӽ� ����	
												D3DPOOL_MANAGED,			// �޸� ���� ���, ���� ������ ��쿣 MANAGED����
												&m_pVB,						// ������ ���ؽ� ���� �İ�ü	
												nullptr)))					// ������ �ڵ�, �⺻�� NULL
		return E_FAIL;
	
	return S_OK;
}

void CVIBuffer::Render_Buffer(void)
{
	// ����̽� �İ�ü���� ���� �׸������ϴ� ���ؽ� ���۸� ��ũ��Ű�� �Լ�
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	// 1. �� �� ������ ���� ����, ����� ���ΰ�
	// 2. ���ؽ����� �����ϴ� ��ü
	// 3. ��𼭺��� �׸��⸦ ������ ���ΰ�(�޸� �����̱� ������ 0byte����)
	// 4. � ũ��� �׸��⸦ ������ ���ΰ�

	// ���� �׸����� ���ؽ��� �Ӽ��� �����ϴ� �Լ�(�� �����Ӹ��� ��������� ��)
	m_pGraphicDev->SetFVF(m_dwFVF);

	// ���� �׸��� �Լ�
	m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriCnt);
	// 1. �ﰢ���� �׸��� ���
	// 2. �� ��° ���ؽ����� �׸��⸦ ������ ���ΰ�
	// 3. �׸��⸦ ������ �ﰢ�� ����

}

void CVIBuffer::Free(void)
{
	Safe_Release(m_pVB);

	Safe_Release(m_pGraphicDev);
}
