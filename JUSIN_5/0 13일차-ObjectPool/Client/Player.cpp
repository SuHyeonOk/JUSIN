#include "stdafx.h"
#include "Player.h"
#include "TextureMgr.h"
#include "Device.h"
#include "AstarMgr.h"
#include "TimeMgr.h"
#include "Bullet.h"
#include "ObjMgr.h"
#include "BulletPoolMgr.h"

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	Release();
}

HRESULT CPlayer::Initialize(void)
{
	m_wstrObjKey = L"Player";
	m_wstrStateKey = L"Dash";

	m_tFrame = {0.f, 10.f };

	return S_OK;
}

int CPlayer::Update(void)
{

	D3DXMATRIX	matScale, matTrans;
	
	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans, 
		m_tInfo.vPos.x + CObj::m_vScroll.x, 
		m_tInfo.vPos.y + CObj::m_vScroll.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;
	
	return OBJ_NOEVENT;
}

void CPlayer::Late_Update(void)
{
	Move_Frame();

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		CAstarMgr::Get_Instance()->Astar_Start(m_tInfo.vPos, Get_Mouse() - CObj::m_vScroll);
	}

	Move_Route();
	Fire_Bullet();
}

void CPlayer::Render(void)
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

void CPlayer::Fire_Bullet(void)
{
	// 3�� ���� �α�
	m_fFireItv += CTimeMgr::Get_Instance()->Get_TimeDelta();
	
	if (m_fFireItv < 3.f) 
		return;

	m_fFireItv = 0.f;

	D3DXMATRIX		matRotZ, matTrans, matWorld;

	D3DXVECTOR3		vDir{ 1.f, 0.f, 0.f };

	for (m_fFireAngle = 0.f; m_fFireAngle < 360.f; ++m_fFireAngle) // 0~359 ����
	{
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(-m_fFireAngle));
		// (-m_fFireAngle) == (-1) �� ���ִ� ����?
		// ȸ�� ��Ŀ� ��� ������ ���� �Ǹ� �̹����� ���ݾ� ȸ�� �� ���·�(���ư��� �� ó�� ����) ��µǰ� �ȴ�
		// (-m_fFireAngle)�� �������ν� �̹����� �������� ��� ��µ� �� �ֵ��� ���ش�
		D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);

		matWorld = matRotZ * matTrans;
		D3DXVec3TransformNormal(&vDir, &vDir, &matWorld); // ���� ����� ���δ�

		//CBullet*	pBullet = CBullet::Create(m_tInfo.vPos, vDir);

		CObj*		pBullet = CBulletPoolMgr::Get_Instance()->Reuse_Obj(m_tInfo.vPos, vDir);

		CObjMgr::Get_Instance()->Add_Object(BULLET, pBullet);

		++m_iCount; // �Ѿ� ����
	}

/*
#ifdef _DEBUG

	cout << "�Ѿ� ���� : " << m_iCount << endl;
#endif // _DEBUG
*/

}
