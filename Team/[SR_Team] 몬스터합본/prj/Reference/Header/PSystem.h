#pragma once

#include "Base.h"
#include "Engine_Include.h"
// 파티클 시스템
// 시스템 내 모든 파티클에 영향을 주는 전역 특성을 관리
// 파티클 갱신, 디스플레이, 소멸, 생성 등을 관장

BEGIN(Engine)

class ENGINE_DLL CPSystem : public CBase
{
	//DECLARE_SINGLETON(CPSystem)

private:
	explicit CPSystem();
	virtual ~CPSystem();

public:									
	virtual _bool	init(IDirect3DDevice9* pGraphicDev, const _tchar * texFileName); // D3D 장치 의존적인 초기화 작업
	virtual void	reset();									// 시스템 내 모든 파티클 속성 리셋
	virtual void	resetParticle(Attribute* attribute) = 0;	// 한 파티클의 속성을 리셋
	virtual void	addParticle();								// 파티클을 추가
	virtual void	update(_float fTimeDelta) = 0;				// 파티클 갱신
	
	virtual void	preRender();		// 렌더링에 앞서 지정할 초기 렌더 상태
	virtual void	render();			// 렌더링
	virtual void	postRender();		// 특정 파티클 시스템이 지정했을 수 있는 렌더 상태를 복구할때 이용

public:
	bool isEmpty();	// 현재 시스템에 파티클이 있으면 True
	bool isDead();	// 시스템 내 모든 파티클이 죽은경우 True

protected:
	virtual void removeDeadParticles();	// 죽은 파티클을 리스트에서 제거

public:
	virtual void Free(void);

protected:
	IDirect3DDevice9*			m_pGarphicDev;
	_vec3						m_origin;			// 시스템의 원천. 파티클이 시작되는 곳
	//BoundingBox					m_boundingBox;		// 파티클이 이동할 수 있는 부피 제한
	_float						m_fEmitRate;		// 시스템에 새로운 파티클이 추가되는 비율. (개수/s)
	_float						m_fSize;			// 시스템 내 모든 파티클의 크기
	IDirect3DTexture9*			m_Tex;
	IDirect3DVertexBuffer9*		m_vb;
	list<Attribute>				m_particles;		// 시스템 내 파티클 속성 리스트
													// 파티클 생성/제거/갱신 시 이 리스트를 이용
													// 파티클을 그릴 준비가 완료되면 리스트 노드 일부를
													// 버텍스 버퍼로 복사하고, 파티클 드로잉 과정을 거침.
													// 모든 파티클을 그릴때까지 이 과정을 반복.
	_int						m_maxParticles;		// 주어진 시간동안 시스템이 가질 수 있는 최대 파티클 수

	_ulong						m_vbSize;			// 버텍스 버퍼가 보관할 수 있는 파티클 수
													// 실제 파티클 시스템 내 파티클 수와는 독립적
	_ulong						m_vbOffset;			// 렌더링에 이용됨
	_ulong						m_vbBatchSize;		// 렌더링에 이용됨

};

END
