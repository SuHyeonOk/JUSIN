#pragma once

#include "Component.h"

BEGIN(Engine)

// VI == Vertex & Index
// ���߿��� VIBuffer�� �ƴ� Component���� ��ӹް� �� ��
class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT		Ready_Buffer(void);
	virtual void		Render_Buffer(void);

protected:

	LPDIRECT3DVERTEXBUFFER9		m_pVB;		// �������� �����ϰ� �����ϱ� ���� �İ�ü
	LPDIRECT3DINDEXBUFFER9		m_pIB;		// �������� �ε��� ���� �����ϱ� ���� �İ�ü

	_ulong		m_dwVtxCnt;			// ���ؽ� ����
	_ulong		m_dwVtxSize;		// ���ؽ� ũ��
						// direct�� ���ؽ���� �ڷ����� ����. ����� ���Ƿ� ������ �ϴµ�,
						// �� ���ؽ��� pos ������, �ؽ���UV ������ ������ ���ִ� �ɼ��� ������.
						// �� ������ ũ�� ��, ����ü�� ũ��� �̷�����.
						// ���ؽ� : pos(vec3 => 12byte) , color (DWORD)
	_ulong		m_dwFVF;			// ���ؽ��� �Ӽ� ������ �����ϴ� �ɼ�
	_ulong		m_dwTriCnt;			// �ﰢ���� ����

	// �ε��� ���۸� ���� �غ�
	_ulong		m_dwIdxSize;	// �ε��� ������ ũ��� �ﰢ�� ������ ������ �޴´�.
	D3DFORMAT	m_IdxFmt;

public:
	virtual void		Free(void);
};

END