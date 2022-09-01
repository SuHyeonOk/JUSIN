#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : 	public CBase
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT		Ready_Buffer(void);
	virtual void		Render_Buffer(void);

protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;

	LPDIRECT3DVERTEXBUFFER9		m_pVB;		// �������� �����ϰ� �����ϱ� ���� �İ�ü
	LPDIRECT3DINDEXBUFFER9		m_pIB;		// �������� �ε��� ���� �����ϱ� ���� �İ�ü

	_ulong		m_dwVtxCnt;			// ���ؽ� ����
	_ulong		m_dwVtxSize;		// ���ؽ� ũ��
	_ulong		m_dwFVF;			// ���ؽ��� �Ӽ� ������ �����ϴ� �ɼ�
	_ulong		m_dwTriCnt;			// �ﰢ���� ����


public:
	virtual void		Free(void);

};

END