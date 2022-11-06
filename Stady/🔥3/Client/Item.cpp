#include "stdafx.h"
#include "Item.h"

#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "InvenMgr.h"

CItem::CItem()
{
	ZeroMemory(&m_ItemInfo, sizeof(ITEMINFO));
}


CItem::~CItem()
{
	Release();
}

HRESULT CItem::Initialize()
{

	m_sTag = "Tag_Key";

	m_wstrObjKey = L"Object";
	m_wstrStateKey = L"Key";

	m_tFrame = { 0.f, 0.f };

	m_tInfo.vPos = { 300.f, 200.f, 0.f };

	m_fRadius = 25.f;
	m_vPoint[0] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[1] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y - m_fRadius, 0.f };
	m_vPoint[2] = { m_tInfo.vPos.x + m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };
	m_vPoint[3] = { m_tInfo.vPos.x - m_fRadius, m_tInfo.vPos.y + m_fRadius, 0.f };

	for (int i = 0; i < 4; ++i)
		m_vOriginPoint[i] = m_vPoint[i];

	return S_OK;
}

int CItem::Update(void)
{
	if (m_bDead)
		return OBJ_DEAD;

	// TODO : 아이템을 먹지 않으면 Tick을 주어 일정시간 후 삭제되도록

	ChoiceStateKey();

	D3DXMATRIX	matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_fScale, m_fScale, m_fScale);
	D3DXMatrixTranslation(&matTrans,
		m_tInfo.vPos.x + CObj::m_vScroll.x,
		m_tInfo.vPos.y + CObj::m_vScroll.y,
		m_tInfo.vPos.z);

	m_tInfo.matWorld = matScale * matTrans;

	for (int i = 0; i < 4; ++i)
	{
		m_vPoint[i] = m_vOriginPoint[i];
		m_vPoint[i] -= {300.f, 200.f, 0.f};
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
	}

	return OBJ_NOEVENT;
}

void CItem::ChoiceStateKey()
{
	switch (m_ItemInfo.iID)
	{
	case 0:
		m_wstrStateKey = L"Key";
		break;

	case 1:
		m_wstrStateKey = L"Calcifer";
		break;

	}
}

void CItem::Late_Update(void)
{
	Move_Frame();

}

void CItem::Render()
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

void CItem::Release(void)
{
}

void CItem::Make_Item(unsigned int iID, D3DXVECTOR3 vPos, float _fImageScale)
{
	CItem* pItem = new CItem;

	if (FAILED(pItem->Initialize()))
	{
		delete pItem;
		pItem = nullptr; // 안 써도 무방
		return;
	}

	pItem->Set_Pos(vPos);
	pItem->Set_ID(iID);
	pItem->Set_ImageScale(_fImageScale);

	CObjMgr::Get_Instance()->Add_Object(ITEM, pItem);
}

void CItem::OnCollision(CObj * pOther)
{
	if ("Tag_Player" == pOther->Get_Tag())
	{
		if (GetAsyncKeyState('Z'))
		{
			CInvenMgr::Get_Instance()->Add_Item(this);
			Kill_Obj();
		}

	}
}