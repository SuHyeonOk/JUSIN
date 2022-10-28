#include "stdafx.h"
#include "..\Header\InventoryUI.h"
#include "AbstFactory.h"
#include "Inventory.h"
/*------------------------
* BackGround
------------------------*/
CInventoryUI::CInventoryUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}

CInventoryUI::~CInventoryUI()
{
}

HRESULT CInventoryUI::Ready_Object()
{

	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_InventoryUI_Texture", L"Proto_InventoryUI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_iRenderPriority = 8;
	return S_OK;
}

_int CInventoryUI::Update_Object(const _float & fTimeDelta)
{
	if (m_bClose)
		return OBJ_NOEVENT;

	CUI::Update_Object(fTimeDelta);
	
	return OBJ_NOEVENT;
}

void CInventoryUI::Render_Object()
{
	
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();



}

void CInventoryUI::Free()
{
	CUI::Free();
}

CInventoryUI * CInventoryUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CInventoryUI* pInstance = new CInventoryUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

