#include "stdafx.h"
#include "BulletPoolMgr.h"

IMPLEMENT_SINGLETON(CBulletPoolMgr)

CBulletPoolMgr::CBulletPoolMgr()
{
}


CBulletPoolMgr::~CBulletPoolMgr()
{
	Release();
}

void CBulletPoolMgr::Collect_Obj(CObj * pObj)
{
	if (nullptr == pObj) // 값이 없으면 return
		return;

	// AddObj와 똑같은 역할이다 
	m_ObjectPool.push_back(pObj); // 값이 있으면 List에 저장
}

CObj * CBulletPoolMgr::Reuse_Obj(const D3DXVECTOR3 & vPos, const D3DXVECTOR3 & vDir)
{
	CObj*		pBullet = nullptr;

	if (m_ObjectPool.empty()) // ﻿아애 m_ObjPool이 비어있는 경우
	{
		// ﻿m_ObjPool에 총알을 만들어서 넣어주어야 한다
		pBullet = CBullet::Create(vPos, vDir);
		++m_iCount;

#ifdef _DEBUG
		cout << "총알 개수 : " << m_iCount << endl;
#endif // _DEBUG

	}

	else
	{
		pBullet = m_ObjectPool.front();
		m_ObjectPool.pop_front();		
	}

	dynamic_cast<CBullet*>(pBullet)->Set_Pos(vPos);
	dynamic_cast<CBullet*>(pBullet)->Set_Dir(vDir);
	
	return pBullet;
}

void CBulletPoolMgr::Release(void)
{
	for_each(m_ObjectPool.begin(), m_ObjectPool.end(), CDeleteObj());
	m_ObjectPool.clear();
}
