#include "stdafx.h"
#include "Player.h"

#include "TextureMgr.h"
#include "Device.h"
#include "AstarMgr.h"
#include "TimeMgr.h"

#include "InvenMgr.h"
#include "Item.h"

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

	m_tCollisionInfo.vPos = { 0.f, 0.f, 0.f };
	m_tCollisionInfo.vLook = { 1.f, -1.f, 0.f };

	m_fRadius = 25.f;
	m_vPoint[0] = { m_tCollisionInfo.vPos.x - m_fRadius, m_tCollisionInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[1] = { m_tCollisionInfo.vPos.x + m_fRadius, m_tCollisionInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[2] = { m_tCollisionInfo.vPos.x + m_fRadius, m_tCollisionInfo.vPos.y + m_fRadius, 0.f };
	m_vPoint[3] = { m_tCollisionInfo.vPos.x - m_fRadius, m_tCollisionInfo.vPos.y + m_fRadius, 0.f };

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

	CollisionUpdate();
	
	return OBJ_NOEVENT;
}

void CPlayer::CollisionUpdate(void)
{
	D3DXMATRIX	matScale_C, matTrans_C;

	D3DXMatrixScaling(&matScale_C, 0.3f, 0.3f, 0.3f);
	D3DXMatrixTranslation(&matTrans_C,
		m_tCollisionInfo.vPos.x + CObj::m_vScroll.x,
		m_tCollisionInfo.vPos.y + CObj::m_vScroll.y,
		m_tCollisionInfo.vPos.z);

	m_tCollisionInfo.matWorld = matScale_C * matTrans_C;

	for (int i = 0; i < 4; ++i)
	{
		m_vPoint[i] = m_vOriginPoint[i];
		m_vPoint[i] -= {0.f, 0.f, 0.f};
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tCollisionInfo.matWorld);
	}
}

void CPlayer::Late_Update(void)
{
	Move_Frame();

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
	if (GetAsyncKeyState('W'))
	{
		// 플레이어 이미지
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.y += m_tInfo.vDir.y * m_fSpeed;

		// 충돌처리 점
		D3DXVec3TransformNormal(&m_tCollisionInfo.vDir, &m_tCollisionInfo.vLook, &m_tCollisionInfo.matWorld);
		m_tCollisionInfo.vPos.y += m_tCollisionInfo.vDir.y * m_fSpeed;
	}

	if (GetAsyncKeyState('S'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.y -= m_tInfo.vDir.y * m_fSpeed;

		D3DXVec3TransformNormal(&m_tCollisionInfo.vDir, &m_tCollisionInfo.vLook, &m_tCollisionInfo.matWorld);
		m_tCollisionInfo.vPos.y -= m_tCollisionInfo.vDir.y * m_fSpeed;
	}

	if (GetAsyncKeyState('A'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.x -= m_tInfo.vDir.x * m_fSpeed;

		D3DXVec3TransformNormal(&m_tCollisionInfo.vDir, &m_tCollisionInfo.vLook, &m_tCollisionInfo.matWorld);
		m_tCollisionInfo.vPos.x -= m_tCollisionInfo.vDir.x * m_fSpeed;
	}

	if (GetAsyncKeyState('D'))
	{
		D3DXVec3TransformNormal(&m_tInfo.vDir, &m_tInfo.vLook, &m_tInfo.matWorld);
		m_tInfo.vPos.x += m_tInfo.vDir.x * m_fSpeed;

		D3DXVec3TransformNormal(&m_tCollisionInfo.vDir, &m_tCollisionInfo.vLook, &m_tCollisionInfo.matWorld);
		m_tCollisionInfo.vPos.x += m_tCollisionInfo.vDir.x * m_fSpeed;
	}


	if (GetAsyncKeyState('I'))
	{
		CItem::Make_Item(0, m_tInfo.vPos);
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
	if("Tag_Monster" == pOther->Get_Tag())
		OnDamaged(pOther);
	//else if ("Tag_Item" == pOther->Get_Tag())
	//	CInvenMgr::Get_Instance()->Add_Item(ITEM_END, )
		 

}

void CPlayer::OnDamaged(CObj * pOther)
{
	// TODO: 데미지를 입는다.
}
