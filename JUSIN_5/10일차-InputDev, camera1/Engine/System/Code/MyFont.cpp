#include "..\..\Header\MyFont.h"

#include "Export_Function.h"

CMyFont::CMyFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pFont(nullptr), m_pSprite(nullptr)
{
	m_pGraphicDev->AddRef();
}


CMyFont::~CMyFont()
{
}

HRESULT CMyFont::Ready_Font(const _tchar * pFontType, const _uint & iWidth, const _uint & iHeight, const _uint & iWeight)
{
	D3DXFONT_DESC Font_Desc;
	ZeroMemory(&Font_Desc, sizeof(D3DXFONT_DESC));

	Font_Desc.CharSet = HANGEUL_CHARSET; // 한글 사용
	lstrcpy(Font_Desc.FaceName, pFontType);
	Font_Desc.Weight = iWeight;
	Font_Desc.Width = iWidth;
	Font_Desc.Height = iHeight;

	// 폰트 컴객체 생성
	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &Font_Desc, &m_pFont)))
	{
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}

	// 스프라이트 컴객체 생성
	FAILED_CHECK_RETURN_MSG(D3DXCreateSprite(m_pGraphicDev, &m_pSprite), E_FAIL, L"Sprite Create Failed");
	
	return S_OK;
}

void CMyFont::Render_Font(const _tchar * pString, const _vec2 * pPos, D3DXCOLOR Color)
{
	RECT	rc{ (_long)pPos->x, (_long)pPos->y };

	// 알파 블렌딩 지원
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	// ﻿DrawTextW의 W는 "유니코드"
	// 스프라이트 컴객체, 문자열, 문자열 길이, 위치(주소), 출력 색상
	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_NOCLIP, Color);

	m_pSprite->End();
}

CMyFont * CMyFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFontType, const _uint & iWidth, const _uint & iHeight, const _uint & iWeight)
{
	CMyFont*		pInstance = new CMyFont(pGraphicDev);

	if (FAILED(pInstance->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CMyFont::Free(void)
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);
	Safe_Release(m_pGraphicDev);
}
