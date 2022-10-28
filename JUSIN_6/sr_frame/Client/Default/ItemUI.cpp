#include "stdafx.h"
#include "ItemUI.h"

CItemUI::CItemUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
, m_bPick(false)
{
}

CItemUI::~CItemUI()
{
}

HRESULT CItemUI::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_InventoryUI_Texture", L"Proto_InventoryUI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_iRenderPriority = 7;
	return S_OK;
}

_int CItemUI::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	if (m_bClose)
		return OBJ_NOEVENT;

	
	CUI::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;
}

void CItemUI::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();

}

void CItemUI::Free()
{
	CUI::Free();
}

CItemUI * CItemUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CItemUI* pInstance = new CItemUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}