#include "stdafx.h"
#include "Player.h"

#include "TextureMgr.h"
#include "Device.h"
#include "AstarMgr.h"
#include "TimeMgr.h"

#include "InvenMgr.h"
#include "Item.h"
#include "ObjMgr.h"
#include "Monster.h"
#include "KeyMgr.h"

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	Release();
}

HRESULT CPlayer::Initialize(void)
{
	m_sTag = "Tag_Player";

	m_wstrObjKey = L"Player";
	m_wstrStateKey = L"Poro";

	m_tFrame = {0.f, 1.f };

	m_fSpeed = 10.f;

	m_tStats.iHp = 100;
	m_tStats.iMaxHp = 100;
	m_tStats.iAttack = 50;





	m_fRadius = 25.f;
	m_vPoint[0] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[1] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[2] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };
	m_vPoint[3] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };

	for (int i = 0; i < 4; ++i)
		m_vOriginPoint[i] = m_vPoint[i];

	return S_OK;
}

int CPlayer::Update(void)
{
	if (m_bDead)
		return OBJ_DEAD;

	Key_Input();

	D3DXMATRIX	matScale, matTrans;
	
	D3DXMatrixScaling(&matScale, 0.3f, 0.3f, 0.3f);
	D3DXMatrixTranslation(&matTrans, 
		m_tInfo.vPos.x + CObj::m_vScroll.x, 
		m_tInfo.vPos.y + CObj::m_vScroll.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;

	for (int i = 0; i < 4; ++i)
	{
		m_vPoint[i] = m_vOriginPoint[i];
		m_vPoint[i] -= {0.f, 0.f, 0.f};
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
	}
	
	return OBJ_NOEVENT;
}

void CPlayer::Late_Update(void)
{
	Move_Frame();
	::Get_Mouse();

	//if (GetAsyncKeyState(VK_LBUTTON))
	//{
	//	CAstarMgr::Get_Instance()->Astar_Start(m_tInfo.vPos, Get_Mouse() - CObj::m_vScroll);
	//}

	//Move_Route();
}

void CPlayer::Render()
{
	const TEXINFO*		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(m_wstrObjKey.c_str(), m_wstrStateKey.c_str(), (int)m_tFrame.fFrame);

	float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
	float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

	CDevice::Get_Instance()->Get_Sprite()->SetTransform(&m_tInfo.matWorld);
		
	CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture, 
		nullptr, 
		&D3DXVECTOR3(fCenterX, fCenterY, 0.f),
		nullptr, 
		D3DCOLOR_ARGB(255, 255, 255, 255)); 

}

void CPlayer::Release(void)
{
	
}

void CPlayer::Key_Input(void)
{
	if (CKeyMgr::Get_Instance()->Key_Up('K'))
	{
		CItem::Make_Item(1, m_tInfo.vPos, 0.2f);
	}
	if (CKeyMgr::Get_Instance()->Key_Down('K'))
	{
		CItem::Make_Item(0, m_tInfo.vPos, 0.5f);
	}


	if (GetAsyncKeyState('W'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.y += m_tInfo.vDir.y * m_fSpeed;
	}

	if (GetAsyncKeyState('S'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.y -= m_tInfo.vDir.y * m_fSpeed;
	}

	if (GetAsyncKeyState('A'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.x -= m_tInfo.vDir.x * m_fSpeed;
	}

	if (GetAsyncKeyState('D'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.x += m_tInfo.vDir.x * m_fSpeed;
	}
}

void CPlayer::Move_Route()
{
	list<TILE*>&	BestList = CAstarMgr::Get_Instance()->Get_BestList();
	
	if (!BestList.empty())
	{
		D3DXVECTOR3	vDir = BestList.front()->vPos - m_tInfo.vPos;

		float	fDistance = D3DXVec3Length(&vDir);

		D3DXVec3Normalize(&vDir, &vDir);

		m_tInfo.vPos += vDir * 300.f * CTimeMgr::Get_Instance()->Get_TimeDelta();

		if (3.f >= fDistance)
		{
			BestList.pop_front();
		}

	}
}

void CPlayer::OnCollision(CObj * pOther)
{
	if ("Tag_Annie" == pOther->Get_Tag())
	{
		if (CKeyMgr::Get_Instance()->Key_Down('L'))
			pOther->OnDamaged(m_tStats.iAttack);
	}
	//else if ("Tag_Item" == pOther->Get_Tag())
	//	CInvenMgr::Get_Instance()->Add_Item(ITEM_END, )
		 
}
