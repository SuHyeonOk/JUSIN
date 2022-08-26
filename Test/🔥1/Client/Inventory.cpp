#include "stdafx.h"
#include "Inventory.h"

#include "TextureMgr.h"
#include "Device.h"
#include "KeyMgr.h"

CInventory::CInventory()
	:m_bOnInven(false)
{
}


CInventory::~CInventory()
{
	Release();
}

HRESULT CInventory::Initialize(void)
{
	m_sTag = "Tag_Inventory";

	m_wstrObjKey = L"Object";
	m_wstrStateKey = L"Inventory";

	m_tFrame = { 0.f, 0.f };

	m_tInfo.vPos = { 400.f, 300.f, 0.f };

	return S_OK;
}

int CInventory::Update(void)
{
	if (m_bDead)
		return OBJ_DEAD;

	if (CKeyMgr::Get_Instance()->Key_Down('I'))
	{
		if (!m_bOnInven)
			m_bOnInven = true;
		else
			m_bOnInven = false;
	}

	D3DXMATRIX	matScale, matTrans;

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixTranslation(&matTrans,
		m_tInfo.vPos.x,
		m_tInfo.vPos.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;

	return OBJ_NOEVENT;
}

void CInventory::Late_Update(void)
{
	Move_Frame();
}

void CInventory::Render()
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

void CInventory::Release(void)
{
}
