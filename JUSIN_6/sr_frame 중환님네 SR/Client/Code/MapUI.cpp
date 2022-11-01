#include "stdafx.h"
#include "..\Header\MapUI.h"
#include "AbstFactory.h"
#include "ServerPacketHandler.h"

CMapUI::CMapUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}


CMapUI::~CMapUI()
{
}

HRESULT CMapUI::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_UI_Texture", L"Proto_UI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_iTexNum = 33;
	m_iRenderPriority = 8;
	return S_OK;
}

_int CMapUI::Update_Object(const _float & fTimeDelta)
{
	if (m_bClose)
	{
		for (auto iRend : m_bRend)
			iRend = false;

		return OBJ_NOEVENT;
	}
		
	if (!m_bWorldSet)
	{
		//Å©¸®ÆÛ ½£
		_vec3 vPos = { WINCX * 0.333f, WINCY * 0.463f , 0.f };
		_float fX = vPos.x - WINCX * 0.5f;
		_float fY = -vPos.y + WINCY * 0.5f;
		_vec3 vSize = { WINCX * 0.04f, WINCX * 0.04f, 0.f };
		_vec3 vSizeHover = { WINCX * 0.045f, WINCX * 0.045f, 0.f };
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::Creeper], vSize, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::CreeperHover], vSizeHover, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });

		//ÄíÅ©
		vPos = { WINCX * 0.6f, WINCY * 0.3f , 0.f };
		fX = vPos.x - WINCX * 0.5f;
		fY = -vPos.y + WINCY * 0.5f;
		vSize = { WINCX * 0.04f, WINCX * 0.04f, 0.f };
		vSizeHover = { WINCX * 0.045f, WINCX * 0.045f, 0.f };
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::Kouku], vSize, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::KoukuHover], vSizeHover, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });
		

		// ÄíÅ© Ã¢(¹è°æ)
		vPos = { WINCX * 0.5f, WINCY * 0.5f , 0.f };
		fX = vPos.x - WINCX * 0.5f;
		fY = -vPos.y + WINCY * 0.5f;
		vSize = { WINCX * 0.8f, WINCX * 0.4f, 0.f };
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::KoukuWindow], vSize, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });
		

		// ÄíÅ© Ã¢(ÀÔÀå)
		vPos = { WINCX * 0.5f, WINCY * 0.81f , 0.f };
		fX = vPos.x - WINCX * 0.5f;
		fY = -vPos.y + WINCY * 0.5f;
		vSize = { WINCX * 0.12f, WINCX * 0.04f, 0.f };
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::KoukuWindowEnter], vSize, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });
	

		//Á¾·á ¹öÆ°
		vPos = { WINCX * 0.93f, WINCY * 0.08f , 0.f };
		fX = vPos.x - WINCX * 0.5f;
		fY = -vPos.y + WINCY * 0.5f;
		vSize = { WINCX * 0.04f, WINCX * 0.04f, 0.f };
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::Close], vSize, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });

		//Online Mode
		vPos = { WINCX * 0.85f, WINCY * 0.9f , 0.f };
		fX = vPos.x - WINCX * 0.5f;
		fY = -vPos.y + WINCY * 0.5f;
		vSize = { WINCX * 0.2f, WINCX * 0.07f, 0.f };
		CGameUtilMgr::MatWorldComposeEuler(m_matWorld[(_int)MapUIWorld::OnlineMode], vSize, { 0.f, 0.f ,0.f }, { fX, fY, 0.f });
		m_bWorldSet = true;
	}

	if (!m_bRend[(_int)MapUIRend::KoukuWindow])
	{
		MouseEvent(m_matWorld[(_int)MapUIWorld::Creeper]);
		MouseEvent(m_matWorld[(_int)MapUIWorld::Kouku]);
		MouseEvent(m_matWorld[(_int)MapUIWorld::Close]);
		MouseEvent(m_matWorld[(_int)MapUIWorld::OnlineMode]);
	}
	else	
		MouseEvent(m_matWorld[(_int)MapUIWorld::KoukuWindowEnter]);

	CUI::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;
}

void CMapUI::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::Creeper]);
	m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::EnterIcon));
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::Kouku]);
	m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::EnterIcon));
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::Close]);
	m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::CloseIcon));
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::OnlineMode]);
	m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::OnlineModeIcon));
	m_pBufferCom->Render_Buffer();

	if (m_bRend[(_int)MapUIRend::CreeperHover])
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::CreeperHover]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::EnterIconHover));
		m_pBufferCom->Render_Buffer();
	}

	if (m_bRend[(_int)MapUIRend::KoukuHover])
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::KoukuHover]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::EnterIconHover));
		m_pBufferCom->Render_Buffer();
	}

	if (m_bRend[(_int)MapUIRend::CloseHover])
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::Close]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::CloseHover));
		m_pBufferCom->Render_Buffer();
	}

	if (m_bRend[(_int)MapUIRend::CloseActive])
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::Close]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::CloseActive));
		m_pBufferCom->Render_Buffer();
	}

	if (m_bRend[(_int)MapUIRend::KoukuWindow])
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::KoukuWindow]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::kouku));
		m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::KoukuWindowEnter]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::koukuEnter));
		m_pBufferCom->Render_Buffer();
	}

	if (m_bRend[(_int)MapUIRend::OnlineHover])
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld[(_int)MapUIWorld::OnlineMode]);
		m_pTextureCom->Set_Texture(static_cast<_int>(MapUITexNum::OnlineModeIconHover));
		m_pBufferCom->Render_Buffer();
	}


	if (!m_bRend[(_int)MapUIRend::KoukuWindow])
	{
		Render_Font(L"Gothic_Bold20", L"Å©¸®ÆÛ ½£",
			&_vec2(WINCX * 0.3f, WINCY *0.5f),
			D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		Render_Font(L"Gothic_Bold20", L"ÄíÅ©¼¼ÀÌÆ°",
			&_vec2(WINCX * 0.56f, WINCY *0.34f),
			D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}

}

void CMapUI::Free()
{
	CUI::Free();
}

CMapUI * CMapUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CMapUI* pInstance = new CMapUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CMapUI::MouseEvent(_matrix & matWorld)
{
	_vec3& vPos = _vec3(matWorld._41, matWorld._42, matWorld._43);
	_vec3& vSize = _vec3(matWorld._11, matWorld._22, matWorld._33);

	_float fX = vPos.x + WINCX * 0.5f;
	_float fY = -vPos.y + WINCY * 0.5f;

	RECT	rcUI = { LONG(fX - vSize.x * 0.5f), LONG(fY - vSize.y * 0.5f), LONG(fX + vSize.x * 0.5f), LONG(fY + vSize.y * 0.5f) };

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if(matWorld == m_matWorld[(_int)MapUIWorld::Creeper])
	{
		if (PtInRect(&rcUI, ptMouse))
		{
			m_bRend[(_int)MapUIRend::CreeperHover] = true;
			if (MouseKeyDown(DIM_LB))
				CSceneFactory::LoadScene("Loading1", "Stage_Default", true, 500);
		}
		else
			m_bRend[(_int)MapUIRend::CreeperHover] = false;
	
	}
	else if (matWorld == m_matWorld[(_int)MapUIWorld::Kouku])
	{
		if (PtInRect(&rcUI, ptMouse))
		{
			m_bRend[(_int)MapUIRend::KoukuHover] = true;

			if (MouseKeyDown(DIM_LB))
			{
				m_bRend[(_int)MapUIRend::KoukuWindow] = true;
			}
		}
		else
			m_bRend[(_int)MapUIRend::KoukuHover] = false;
	}
	else if (matWorld == m_matWorld[(_int)MapUIWorld::OnlineMode])
	{
		if (PtInRect(&rcUI, ptMouse))
		{
			m_bRend[(_int)MapUIRend::OnlineHover] = true;

			if (MouseKeyDown(DIM_LB))
			{
				CSceneFactory::LoadScene("Loading1", "NetStage_Start", true, 500);
			}
		}
		else
			m_bRend[(_int)MapUIRend::OnlineHover] = false;
	}
	else if(matWorld == m_matWorld[(_int)MapUIWorld::Close])
	{
		if (PtInRect(&rcUI, ptMouse))
		{
			m_bRend[(_int)MapUIRend::CloseHover] = true;

			if (MouseKeyPress(DIM_LB))
			{
				m_bRend[(_int)MapUIRend::CloseActive] = true;
				m_bRend[(_int)MapUIRend::CloseHover] = false;
			}
			else if (MouseKeyUp(DIM_LB))
			{
				m_bClose = true;
			}
			else
			{
				m_bRend[(_int)MapUIRend::CloseActive] = false;
			}
		}
		else
		{
			m_bRend[(_int)MapUIRend::CloseHover] = false;
		}
	}
	else if (matWorld == m_matWorld[(_int)MapUIWorld::KoukuWindowEnter])
	{
		if (PtInRect(&rcUI, ptMouse) && MouseKeyDown(DIM_LB))
		{
			if (g_bOnline)
			{
				Protocol::C_PLAYER_MOVE_STAGE moveStage;
				moveStage.set_loadingtag("KoukuLoading");
				moveStage.set_stagetag("Stage_Kouku");
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(moveStage));	
			}
		}
		
	}
			
}

