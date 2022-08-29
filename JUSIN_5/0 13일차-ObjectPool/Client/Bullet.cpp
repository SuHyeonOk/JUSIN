#include "stdafx.h"
#include "Bullet.h"
#include "TextureMgr.h"
#include "Device.h"
#include "TimeMgr.h"
#include "BulletPoolMgr.h"


CBullet::CBullet()
{
}


CBullet::~CBullet()
{
	Release();
}

HRESULT CBullet::Initialize(void)
{
	
	return S_OK;
}

int CBullet::Update(void)
{
	if (0.f >= m_tInfo.vPos.x + CObj::m_vScroll.x ||
		WINCX <= m_tInfo.vPos.x + CObj::m_vScroll.x ||
		0.f >= m_tInfo.vPos.y + CObj::m_vScroll.y ||
		WINCY <= m_tInfo.vPos.y + CObj::m_vScroll.y)
	{

		CBulletPoolMgr::Get_Instance()->Collect_Obj(this);
		return OBJ_DEAD;
	}

		
	D3DXMATRIX	matScale, matTrans;

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans,
		m_tInfo.vPos.x + CObj::m_vScroll.x,
		m_tInfo.vPos.y + CObj::m_vScroll.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;





	return OBJ_NOEVENT;
}

void CBullet::Late_Update(void)
{
	Move_Bullet();
}

void CBullet::Render(void)
{
	const TEXINFO*		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Effect", L"ElectronicShock" , (int)m_tFrame.fFrame);

	float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
	float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

	CDevice::Get_Instance()->Get_Sprite()->SetTransform(&m_tInfo.matWorld);

	CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,
		nullptr,
		&D3DXVECTOR3(fCenterX, fCenterY, 0.f),
		nullptr,
		D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CBullet::Release(void)
{
	
}

void CBullet::Move_Bullet(void)
{
	m_tInfo.vPos += m_tInfo.vDir * 300.f * CTimeMgr::Get_Instance()->Get_TimeDelta();
}

CBullet* CBullet::Create(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir)
{
	CBullet*		pBullet = new CBullet;

	if (FAILED(pBullet->Initialize()))
	{
		Safe_Delete(pBullet);
		return nullptr;
	}

	pBullet->Set_Pos(vPos);
	pBullet->Set_Dir(vDir);

	return pBullet;
}

