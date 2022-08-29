#pragma once

#include "Bullet.h"

class CBulletPoolMgr
{
	DECLARE_SINGLETON(CBulletPoolMgr)

private:
	CBulletPoolMgr();
	~CBulletPoolMgr();


public:
	void		Collect_Obj(CObj* pObj);										// 사용을 마친 오브젝트를 회수하는 함수
	CObj*		Reuse_Obj(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);	// 현재 풀에 있는 오브젝트를 재사용하는 함수

private:
	void		Release(void);

private:
	list<CObj*>			m_ObjectPool; // 생성된 오브젝트 Pool관리 목적 List
	int					m_iCount = 0;

};

