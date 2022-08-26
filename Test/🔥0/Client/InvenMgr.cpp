#include "stdafx.h"
#include "InvenMgr.h"

IMPLEMENT_SINGLETON(CInvenMgr)

CInvenMgr::CInvenMgr()
{
}


CInvenMgr::~CInvenMgr()
{
}

void CInvenMgr::Add_Item(CItem * pItem)
{
	if (nullptr == pItem)
		return;

	CItem::ITEMINFO Info = pItem->Get_ItemInfo();

	m_listItem.push_back(Info);
}

