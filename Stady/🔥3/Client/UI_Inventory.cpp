#include "stdafx.h"
#include "UI_Inventory.h"

#include "TextureMgr.h"
#include "Device.h"
#include "KeyMgr.h"

#include "ObjMgr.h"
#include "UI_InventoryDregBar.h"

CUI_Inventory::CUI_Inventory()
	:m_bOnInven(false)
{
	m_pInventoryDregBar = nullptr;
}


CUI_Inventory::~CUI_Inventory()
{
	Release();
}

HRESULT CUI_Inventory::Initialize(void)
{
	m_sTag = "Tag_Inventory";

	m_wstrObjKey = L"Object";
	m_wstrStateKey = L"Inventory";

	m_tFrame = { 0.f, 0.f };

	m_tInfo.vPos = { 400.f, 300.f, 0.f };

	m_fRadius = 200.f;
	m_vPoint[0] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[1] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[2] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };
	m_vPoint[3] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };

	for (int i = 0; i < 4; ++i)
		m_vOriginPoint[i] = m_vPoint[i];

	if (nullptr == m_pInventoryDregBar)
	{
		m_pInventoryDregBar = new CUI_InventoryDregBar;
		m_pInventoryDregBar->Initialize();

		// Add_Object을 하지 않고 CUI_InventoryDregBar의 모든 함수는 CUI_Inventory가 호출한다
		//CObjMgr::Get_Instance()->Add_Object(UUI, m_pInventoryDregBar);
	}	

	return S_OK;
}

int CUI_Inventory::Update(void)
{
	if (m_bDead)
		return OBJ_DEAD;

	m_pInventoryDregBar->Update();

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
			m_vPoint[i] -= {400.f, 300.f, 0.f};
		else
			m_vPoint[i] -= {400.f, -3000.f, 0.f};

		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
	}

	return OBJ_NOEVENT;
}

void CUI_Inventory::Late_Update(void)
{
	Move_Frame();

	m_pInventoryDregBar->Late_Update();
}

void CUI_Inventory::Render()
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

		m_pInventoryDregBar->Render();
	}
}

void CUI_Inventory::Release(void)
{
	Safe_Delete(m_pInventoryDregBar);
}

void CUI_Inventory::Key_Input(void)
{
	if (CKeyMgr::Get_Instance()->Key_Down('I'))
	{
		if (!m_bOnInven)
			m_bOnInven = true;
		else
			m_bOnInven = false;

	}
}

void CUI_Inventory::OnCollision(CObj * pOther)
{
	if ("Tag_Inventory" == pOther->Get_Tag())
	{
		m_pInventoryDregBar->Check_Collision();
	}


}