#include "stdafx.h"
#include "..\Header\Rune.h"

CRune::CRune(LPDIRECT3DDEVICE9 pGraphicDev): CEquipItem(pGraphicDev)
{
}

CRune::~CRune()
{
}

HRESULT CRune::Ready_Object()
{
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	
	return S_OK;
}

void CRune::Free()
{
	CEquipItem::Free();
}

