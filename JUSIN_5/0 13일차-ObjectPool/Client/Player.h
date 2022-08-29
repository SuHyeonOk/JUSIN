#pragma once
#include "Obj.h"
class CPlayer :	public CObj
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	virtual HRESULT Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(void) override;
	virtual void Release(void) override;

	// Player.h
private:
	void		Move_Route();
	void		Fire_Bullet(void);

private:
	float		m_fFireItv = 0.f;		// 시간적인 간격(Interval)을 둘 변수
	float		m_fFireAngle = 0.f;		// 360도로 돌기위해 사용할 변수

	int			m_iCount = 0;			// 총알이 몇 발 나가는지 세는 변수

};

// 메모리 단편화 : 조각난 메모리로 인해 메모리가 낭비되거나 충분한 메모리가 있음에도 사용할 수 없는 현상

//- 내부 단편화 : 실제 필요한 메모리보다 초가되는 크기로 할당되어 메모리를 사용하지 못하는 상황(EX) 구조체
//- 외부 단편화 : 빈번한 메모리 할당, 해제 시 메모리가 중간중간 비어 메모리를 사용하지 못하는 상황(EX) 디스크 조각 모음

// 메모리 풀링