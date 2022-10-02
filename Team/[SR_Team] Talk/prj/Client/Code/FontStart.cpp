#include "stdafx.h"
#include "..\Header\FontStart.h"

#include "Export_Function.h"

CFontStart::CFontStart(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CFontStart::~CFontStart()
{
}

HRESULT CFontStart::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Scale(0.5f, 0.04f, 0.5f);
	m_pTransCom->Set_Pos(0.f, -0.5f, 0.f);

	return S_OK;
}

_int CFontStart::Update_Object(const _float & fTimeDelta)
{
	m_fTimeTrueAcc += fTimeDelta;
	m_fTimeFalseAcc += fTimeDelta;
	if (0.5f < m_fTimeTrueAcc)
	{
		m_bRender = true;
		m_fTimeTrueAcc = 0;
	}
	else
	{
		if (1.f < m_fTimeFalseAcc)
		{
			m_bRender = false;
			m_fTimeFalseAcc = 0;
		}
	}

	CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_PRIORITY, this);

	return 0;
}

void CFontStart::LateUpdate_Object(void)
{
	CGameObject::LateUpdate_Object();
}

void CFontStart::Render_Obejct(void)
{
	if (m_bRender)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());

		//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		m_pTextureCom->Set_Texture(0);
		m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}

HRESULT CFontStart::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_LogoTextTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LogoTextTexture", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	return S_OK;
}

CFontStart * CFontStart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFontStart *	pInstance = new CFontStart(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CFontStart::Free(void)
{
	CGameObject::Free();
}
