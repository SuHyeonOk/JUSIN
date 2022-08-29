#pragma once

#include "Bullet.h"

class CBulletPoolMgr
{
	DECLARE_SINGLETON(CBulletPoolMgr)

private:
	CBulletPoolMgr();
	~CBulletPoolMgr();


public:
	void		Collect_Obj(CObj* pObj);										// ����� ��ģ ������Ʈ�� ȸ���ϴ� �Լ�
	CObj*		Reuse_Obj(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);	// ���� Ǯ�� �ִ� ������Ʈ�� �����ϴ� �Լ�

private:
	void		Release(void);

private:
	list<CObj*>			m_ObjectPool; // ������ ������Ʈ Pool���� ���� List
	int					m_iCount = 0;

};

