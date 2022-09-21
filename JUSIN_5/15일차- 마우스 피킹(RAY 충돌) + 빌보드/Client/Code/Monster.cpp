#include "stdafx.h"
#include "..\Header\Monster.h"

#include "Export_Function.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}


CMonster::~CMonster()
{
}

void CMonster::Free(void)
{
	CGameObject::Free();
}
