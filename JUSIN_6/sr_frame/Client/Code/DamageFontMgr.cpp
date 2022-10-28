#include "stdafx.h"
#include "..\Header\DamageFontMgr.h"

IMPLEMENT_SINGLETON(CDamageFontMgr)

CDamageFontMgr::CDamageFontMgr()
{
	m_fSpeed = 1.5f;
}

CDamageFontMgr::~CDamageFontMgr()
{
	Free();
}

void CDamageFontMgr::Update_DamageFontMgr(const _float& fTimeDelta)
{
	for (auto itr = m_vecDamageFont.begin(); itr != m_vecDamageFont.end();)
	{
		if (itr->fTime <= 0.f)
		{
			itr = m_vecDamageFont.erase(itr);
		}
		else
		{
			itr->vPos += itr->vDir * fTimeDelta * m_fSpeed;
			if (itr->fTime < 0.15f)
				itr->Color.a = itr->fTime / 0.15f;
			itr->fTime -= fTimeDelta;
			++itr;
		}
	}

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matProj);
	ZeroMemory(&m_ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&m_ViewPort);
}

void CDamageFontMgr::Add_DamageFontFromWorld(_int iDamage, const _vec3& vPos, const _vec3& vFrom, D3DXCOLOR Color, _bool bCritical)
{
	_vec2 vScreen;
	_vec2 vFromScreen;

	CGameUtilMgr::World2Screen(vScreen, vPos, m_matView, m_matProj, m_ViewPort);
	CGameUtilMgr::World2Screen(vFromScreen, vFrom, m_matView, m_matProj, m_ViewPort);


	_vec2 vDir = vScreen - vFromScreen;

	for (auto& damageFont : m_vecDamageFont)
	{
		const _vec2 vDiff = vScreen - damageFont.vPos;
		if (D3DXVec2LengthSq(&vDiff) < 100.f)
			damageFont.vPos += damageFont.vDir * 0.15f * m_fSpeed;
	}

	m_vecDamageFont.push_back({iDamage, vDir, vScreen, Color, bCritical});
}

void CDamageFontMgr::Add_DamageFontFromScreen(_int iDamage, const _vec2& vScreen, D3DXCOLOR Color)
{
	m_vecDamageFont.push_back({iDamage, {0.f, 0.f}, vScreen, Color, false});
}

void CDamageFontMgr::Render_DamageFontMgr()
{
	for (const auto& damageFont : m_vecDamageFont)
	{
		if (damageFont.bCritical)
		{
			Engine::Render_Font(
				L"Gothic_Bold30", 
				to_wstring(damageFont.iDamage).c_str(),
				&damageFont.vPos,
				D3DCOLOR_ARGB(255, 255, 0, 0));
		}
		else
		{
			Engine::Render_Font(
				L"Gothic_Bold20", 
				to_wstring(damageFont.iDamage).c_str(),
				&damageFont.vPos,
				damageFont.Color);	
		}
	}
}

void CDamageFontMgr::Free()
{
	Safe_Release(m_pGraphicDev);
	m_vecDamageFont.clear();
}
