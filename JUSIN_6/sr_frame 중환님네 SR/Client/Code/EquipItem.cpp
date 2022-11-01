#include "stdafx.h"
#include "..\Header\EquipItem.h"

CEquipItem::CEquipItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:CItem(pGraphicDev)
	, m_pItemUI(nullptr)
{
	m_eItemType = IT_END;
	m_iAttackCnt = 0;
	m_bUse = false;
	m_iUItexNum = 0;
}



CEquipItem::~CEquipItem()
{
}

HRESULT CEquipItem::Ready_Object()
{
	return S_OK;
}

_int CEquipItem::Update_Object(const _float & fTimeDelta)
{
	CItem::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);
	return 0;
}

void CEquipItem::LateUpdate_Object()
{
	CItem::LateUpdate_Object();
}

void CEquipItem::Render_Object()
{
}


void CEquipItem::Free()
{
	CItem::Free();
}

void CEquipItem::SetDelete()
{
	CItem::SetDelete();
	m_pItemUI->SetDelete();
}

