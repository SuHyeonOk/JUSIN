#include "stdafx.h"
#include "Obj.h"
#include "TimeMgr.h"

D3DXVECTOR3		CObj::m_vScroll{};

CObj::CObj()
	: m_wstrObjKey(L""), m_wstrStateKey(L""), m_bDead(false)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));	
	D3DXMatrixIdentity(&m_tInfo.matWorld);

	ZeroMemory(&m_tFrame, sizeof(FRAME));
	m_tInfo.vLook = D3DXVECTOR3(1.f, -1.f, 0.f);
}

CObj::~CObj()
{
}

void CObj::OnDamaged(int _iAttack)
{
	m_tStats.iHp -= _iAttack;
	if (0 >= m_tStats.iHp)
	{
		m_tStats.iHp = 0;
		OnDead();
	}
}

void CObj::Move_Frame(void)
{
	m_tFrame.fFrame += m_tFrame.fMax * CTimeMgr::Get_Instance()->Get_TimeDelta();

	if (m_tFrame.fFrame > m_tFrame.fMax)
		m_tFrame.fFrame = 0.f;
}
