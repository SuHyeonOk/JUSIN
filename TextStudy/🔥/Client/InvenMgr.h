#pragma once

#include "Include.h"
#include "Item.h"

class CInvenMgr
{
	DECLARE_SINGLETON(CInvenMgr)

public:
	CInvenMgr();
	~CInvenMgr();

public:
	void			Add_Item(CItem* pItem);

private:
	list<CItem::ITEMINFO>			m_listItem;

};

