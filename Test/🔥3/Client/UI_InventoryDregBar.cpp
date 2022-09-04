#include "stdafx.h"
#include "UI_InventoryDregBar.h"

#include "TextureMgr.h"
#include "Device.h"
#include "KeyMgr.h"

#include "CollisionMgr.h"


CUI_InventoryDregBar::CUI_InventoryDregBar()
	:m_bOnInven(false), m_bCheckMouse(false)
{
}


CUI_InventoryDregBar::~CUI_InventoryDregBar()
{
	Release();
}

HRESULT CUI_InventoryDregBar::Initialize(void)
{
	m_sTag = "Tag_InventoryDregBar";

	m_wstrObjKey = L"Object";
	m_wstrStateKey = L"Items";

	m_tFrame = { 0.f, 0.f };

	m_tInfo.vPos = { 400.f, 115.f, 0.f };

	m_fRadius = 50.f;
	m_vPoint[0] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[1] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[2] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };
	m_vPoint[3] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };

	for (int i = 0; i < 4; ++i)
		m_vOriginPoint[i] = m_vPoint[i];

	return S_OK;
}

int CUI_InventoryDregBar::Update(void)
{
	if (m_bDead)
		return OBJ_DEAD;

	Key_Input();

	D3DXMATRIX	matScale, matTrans;

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans,
		m_tInfo.vPos.x,
		m_tInfo.vPos.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;

	// 보이지 않는 콜리전 박스
	for (int i = 0; i < 4; ++i)
	{
		m_vPoint[i] = m_vOriginPoint[i];

		if (m_bOnInven)
			m_vPoint[i] -= {400.f, 115.f, 0.f};
		else
			m_vPoint[i] -= {400.f, -3000.f, 0.f};

		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
	}

	return OBJ_NOEVENT;
}

void CUI_InventoryDregBar::Late_Update(void)
{
	Move_Frame();
}

void CUI_InventoryDregBar::Render()
{
	if (m_bOnInven)
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
}

void CUI_InventoryDregBar::Release(void)
{
}

void CUI_InventoryDregBar::Key_Input(void)
{
	if (CKeyMgr::Get_Instance()->Key_Down('I'))
	{
		if (!m_bOnInven)
			m_bOnInven = true;
		else
			m_bOnInven = false;
	}


}

void CUI_InventoryDregBar::Check_Collision()
{
	if (CCollisionMgr::Check_Rect_Mouse(this))
		m_bCheckMouse = true;

	OnCollision(this);
}

void CUI_InventoryDregBar::OnCollision(CObj * pOther)
{
	if (m_bCheckMouse)
	{
		if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
		{
			m_tInfo.vPos += Get_Mouse();
			
		}
	}
}