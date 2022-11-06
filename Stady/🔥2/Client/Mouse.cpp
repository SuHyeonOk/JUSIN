#include "stdafx.h"
#include "Mouse.h"

#include "TextureMgr.h"
#include "Device.h"

CMouse::CMouse()
{
}


CMouse::~CMouse()
{
}

HRESULT CMouse::Initialize(void)
{
	m_sTag = "Tag_Mouse";

	m_wstrObjKey = L"Object";
	m_wstrStateKey = L"MousePointer";

	m_tFrame = { 0.f, 0.f };

	m_tInfo.vPos = { 400.f, 300.f, 0.f };

	//m_fRadius = 0.5f;
	//m_vPoint[0] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	//m_vPoint[1] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	//m_vPoint[2] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };
	//m_vPoint[3] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };

	//for (int i = 0; i < 4; ++i)
	//	m_vOriginPoint[i] = m_vPoint[i];

	return S_OK;
}

int CMouse::Update(void)
{
	if (m_bDead)
		return OBJ_DEAD;

	ShowCursor(false); // 마우스 커서 지우기

	D3DXMATRIX	matScale, matTrans;

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans,
		::Get_Mouse().x,
		::Get_Mouse().y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;

	//for (int i = 0; i < 4; ++i)
	//{
	//	m_vPoint[i] = m_vOriginPoint[i];
	//	m_vPoint[i] -= {300.f, 400.f, 0.f};
	//	D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
	//}

	return OBJ_NOEVENT;
}

void CMouse::Late_Update(void)
{
	Move_Frame();
}

void CMouse::Render()
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

void CMouse::Release(void)
{
}


void CMouse::OnCollision(CObj * pOther)
{
}