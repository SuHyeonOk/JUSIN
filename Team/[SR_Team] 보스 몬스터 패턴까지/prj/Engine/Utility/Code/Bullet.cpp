#include "..\..\Header\Bullet.h"

USING(Engine)
CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	//m_pGraphicDev->AddRef();
}

CBullet::CBullet(const CBullet & rhs)
	: CGameObject(rhs)
{
	//m_pGraphicDev->AddRef();
}

CBullet::~CBullet()
{
}

void CBullet::Free(void)
{
	CGameObject::Free(); 
	//Safe_Release(m_pGraphicDev);
}