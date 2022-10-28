#include "stdafx.h"
#include "..\Header\Portrait.h"
#include "AbstFactory.h"

CPortrait::CPortrait(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}


CPortrait::~CPortrait()
{
}


HRESULT CPortrait::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTargetTexture = CTargetTexture::Create(m_pGraphicDev);

	m_Idle = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_idle.anim");
	m_Dance = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/mvp_pose.anim");

	return S_OK;
}

_int CPortrait::Update_Object(const _float& fTimeDelta)
{
	for (auto e : m_arrPlayer)
	{
		m_pTargetTexture->AddRender(e);
	}
	return CUI::Update_Object(fTimeDelta);
}

void CPortrait::LateUpdate_Object()
{
	_matrix matViewOrigin;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matViewOrigin);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matPortraitCam);
	m_pTargetTexture->RenderOnSurface();

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matViewOrigin);
}

void CPortrait::Render_Object()
{
	if (m_bVisible == false) return;

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTargetTexture->Set_Texture();
	m_pBufferCom->Render_Buffer();

	if (m_wstrText.empty() == false)
	{
		const _vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
		const _float fX = vPos.x + WINCX / 2;
		const _float fY = -vPos.y + WINCY / 2;

		Render_Font(m_wstrFont, m_wstrText.c_str(), &_vec2{fX + m_vTextOffset.x, fY + m_vTextOffset.y}, m_TextColor);
	}
	

	Render_Font(L"Gothic_Bold70", L"MVP", &_vec2{795 ,420}, D3DCOLOR_ARGB(255, 10, 230, 240));
	Render_Font(L"Gothic_Bold30", m_arrPlayerName[PLAYER_MVP].c_str(), &_vec2{845 ,500}, D3DCOLOR_ARGB(255, 255, 255, 255));
	Render_Font(L"Gothic_Bold20", L"ÀÜÈ¤ÇÑ Ç÷Åõ»ç", &_vec2{740 ,610}, D3DCOLOR_ARGB(255, 255, 255, 200));
	Render_Font(L"Gothic_Bold20", L"ÁØ ÇÇÇØ", &_vec2{770 ,630}, D3DCOLOR_ARGB(255, 255, 255, 255));
	wstring damage = to_wstring((_uint)m_arrPlayerDamage[PLAYER_MVP]) + L"%";
	Render_Font(L"Gothic_Bold20", damage.c_str(), &_vec2{780 ,650}, D3DCOLOR_ARGB(255, 255, 255, 0));

	wstring cnt = to_wstring(m_arrPlayerCnt[PLAYER_MVP]) + L"¹ø";
	Render_Font(L"Gothic_Bold20", L"Ä«¿îÅÍ ±Í½Å", &_vec2{950 ,610}, D3DCOLOR_ARGB(255, 255, 255, 200));
	Render_Font(L"Gothic_Bold20", L"Ä«¿îÅÍ È½¼ö", &_vec2{950 ,630}, D3DCOLOR_ARGB(255, 255, 255, 255));
	Render_Font(L"Gothic_Bold20", cnt.c_str(), &_vec2{990 ,650}, D3DCOLOR_ARGB(255, 255, 255, 0));

	Render_Font(L"Gothic_Bold20", m_arrPlayerName[PLAYER_COUNTER].c_str(), &_vec2{670 ,175}, D3DCOLOR_ARGB(255, 255, 255, 255));
	Render_Font(L"Gothic_Bold20", m_arrPlayerName[PLAYER_WALK].c_str(), &_vec2{610 ,425}, D3DCOLOR_ARGB(255, 255, 255, 255));
	Render_Font(L"Gothic_Bold20", m_arrPlayerName[PLAYER_POTION_].c_str(), &_vec2{550 ,675}, D3DCOLOR_ARGB(255, 255, 255, 255));

}

CPortrait* CPortrait::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CPortrait* pInstance = new CPortrait(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPortrait::Free()
{
	Safe_Release(m_pTargetTexture);
	CUI::Free();
}

void CPortrait::SetPlayerSkeletal(Protocol::PlayerSkin eSkin, PLAYER_MVP_TYPE eMvp, _float fDamagePercent, _uint iCnt,
                                  const wstring& wstrName)
{
	CGameObject* pGameObject = nullptr;
	_matrix matWorld;
	CSkeletalCube* pTarget = nullptr;

	switch (eMvp)
	{
	case PLAYER_MVP:
		CGameUtilMgr::MatWorldComposeEuler(matWorld, {1.5f, 1.5f, 1.5f}, {0.f, D3DXToRadian(30.f), 0.f}, {-3.f, -1.5f, 0.f});
		break;
	case PLAYER_COUNTER:
		CGameUtilMgr::MatWorldComposeEuler(matWorld, {0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, {-0.5f, 1.5f, 0.f});
		break;
	case PLAYER_WALK:
		CGameUtilMgr::MatWorldComposeEuler(matWorld, {0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.f, -0.5f, 0.f});
		break;
	case PLAYER_POTION_:
		CGameUtilMgr::MatWorldComposeEuler(matWorld, {0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f}, {0.5f, -2.5f, 0.f});
		break;
	case PLAYER_MVP_END:
		break;
	default: ;
	}


	CGameUtilMgr::MatWorldComposeEuler(m_matPortraitCam, {1.f, 1.f, 1.f}, {0.f, D3DXToRadian(180.f), 0.f},
	                                   {0.f, 0.f, 5.f});
	D3DXMatrixInverse(&m_matPortraitCam, nullptr, &m_matPortraitCam);

	switch (eSkin)
	{
	case Protocol::PLAYER_TYPE_STEVE:
		pGameObject = pTarget =
			CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Steve.cube");
		Get_Layer(LAYER_ENV)->Add_GameObject(L"SteveDummy", pGameObject);
		break;
	case Protocol::PLAYER_TYPE_PRIDE:
		pGameObject = pTarget =
			CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Pride.cube");
		Get_Layer(LAYER_ENV)->Add_GameObject(L"PrideDummy", pGameObject);
		break;
	case Protocol::PLAYER_TYPE_ESHE:
		pGameObject = pTarget = CSkeletalCube::Create(m_pGraphicDev, L"../Bin/Resource/SkeletalCube/CubeMan/Eshe.cube");
		Get_Layer(LAYER_ENV)->Add_GameObject(L"EsheDummy", pGameObject);
		break;
	case Protocol::PLAYER_TYPE_COPPER:
		pGameObject = pTarget = CSkeletalCube::Create(m_pGraphicDev,
		                                              L"../Bin/Resource/SkeletalCube/CubeMan/Copper.cube");
		Get_Layer(LAYER_ENV)->Add_GameObject(L"CopperDummy", pGameObject);
		break;
	case Protocol::PlayerSkin_INT_MIN_SENTINEL_DO_NOT_USE_: break;
	case Protocol::PlayerSkin_INT_MAX_SENTINEL_DO_NOT_USE_: break;
	default: ;
	}

	pGameObject->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->Set_WorldDecompose(matWorld);
	pTarget->PlayAnimationLoop(&m_Idle);
	pTarget->Get_SkeletalPart("weapon_r")->pBuf = nullptr;
	pTarget->SetLight(false);
	m_arrPlayer[eMvp] = pTarget;
	m_arrPlayerDamage[eMvp] = fDamagePercent;
	m_arrPlayerCnt[eMvp] = iCnt;
	m_arrPlayerName[eMvp] = wstrName;
	if (eMvp == PLAYER_MVP)pTarget->PlayAnimationLoop(&m_Dance);
}
