#include "stdafx.h"
#include "..\Header\ConsumeItem.h"


CConsumeItem::CConsumeItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:CItem(pGraphicDev)
{
	m_eItemEffect = IE_END;
	m_bDead = false;
}


CConsumeItem::~CConsumeItem()
{
}

HRESULT CConsumeItem::Ready_Object()
{
	return S_OK;
}

_int CConsumeItem::Update_Object(const _float & fTimeDelta)
{
	
	CItem::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);
	return 0;
}

void CConsumeItem::LateUpdate_Object()
{
	CItem::LateUpdate_Object();
}

void CConsumeItem::Render_Object()
{
}

void CConsumeItem::Free()
{
	CItem::Free();
}
