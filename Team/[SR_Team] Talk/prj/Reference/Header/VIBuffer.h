#pragma once

#include "Component.h"

BEGIN(Engine)

// VI == Vertex & Index
// 나중에는 VIBuffer가 아닌 Component에서 상속받게 될 것
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

	LPDIRECT3DVERTEXBUFFER9		m_pVB;		// 정점들을 보관하고 접근하기 위한 컴객체
	LPDIRECT3DINDEXBUFFER9		m_pIB;		// 정점들의 인덱스 값을 보관하기 위한 컴객체

	_ulong		m_dwVtxCnt;			// 버텍스 개수
	_ulong		m_dwVtxSize;		// 버텍스 크기
						// direct는 버텍스라는 자료형이 없다. 사용자 정의로 만들어야 하는데,
						// 이 버텍스가 pos 용인지, 텍스쳐UV 용인지 구분을 해주는 옵션은 제공함.
						// 그 구분이 크기 즉, 구조체의 크기로 이뤄진다.
						// 버텍스 : pos(vec3 => 12byte) , color (DWORD)
	_ulong		m_dwFVF;			// 버텍스의 속성 정보를 저장하는 옵션
	_ulong		m_dwTriCnt;			// 삼각형의 개수

	// 인덱스 버퍼를 위한 준비
	_ulong		m_dwIdxSize;	// 인덱스 버퍼의 크기는 삼각형 개수에 영향을 받는다.
	D3DFORMAT	m_IdxFmt;

public:
	virtual void		Free(void);
};

END