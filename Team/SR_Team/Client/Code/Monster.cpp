#include "stdafx.h"
#include "..\Header\Monster.h"

#include "Export_Function.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_fTimeDelta(0.f)
	, m_fFrame(0.f)

{
	ZeroMemory(&m_vDirection, sizeof(_vec3));
}


CMonster::~CMonster()
{
}

void CMonster::Free(void)
{
	CGameObject::Free();
}
