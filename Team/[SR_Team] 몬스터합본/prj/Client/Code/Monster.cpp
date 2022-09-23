#include "stdafx.h"
#include "..\Header\Monster.h"

#include "Export_Function.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_fFrame(0.f)
	, m_fSpeed(0.f)

{
}


CMonster::~CMonster()
{
}

void CMonster::Free(void)
{
	CGameObject::Free();
}
