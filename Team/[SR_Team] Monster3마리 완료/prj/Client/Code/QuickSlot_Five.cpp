#include "stdafx.h"
#include "..\Header\QuickSlot_Five.h"

#include "Export_Function.h"

CQuickSlot_Five::CQuickSlot_Five(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
}

CQuickSlot_Five::~CQuickSlot_Five()
{
}

HRESULT CQuickSlot_Five::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matWorld);

	// 스케일 값
	D3DXMatrixScaling(&m_matView, 32.f, 32.f, 1.f);

	// 포지션
	m_matView._41 = QUICKSLOT_FIVE_X;
	m_matView._42 = QUICKSLOT_Y;

	return S_OK;
}

_int CQuickSlot_Five::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CQuickSlot_Five::LateUpdate_Object(void)
{
	Engine::CGameObject::LateUpdate_Object();
}

void CQuickSlot_Five::Render_Obejct(void)
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(0);	// 텍스처 정보 세팅을 우선적으로 한다.
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CQuickSlot_Five::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// m_pTextureCom	
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UI_QuickSlot5_Texture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_UI_QuickSlot5_Texture", pComponent });
	
	return S_OK;
}

CQuickSlot_Five * CQuickSlot_Five::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CQuickSlot_Five *	pInstance = new CQuickSlot_Five(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CQuickSlot_Five::Free(void)
{
	CUI::Free();
}
