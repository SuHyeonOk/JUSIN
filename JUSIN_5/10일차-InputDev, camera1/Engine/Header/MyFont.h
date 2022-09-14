#pragma once
#include "Base.h"
#include "Engine_Include.h"

class ENGINE_DLL CMyFont : public CBase
{
private:
	explicit CMyFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMyFont();

public:
	HRESULT		Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);
	void		Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;	// 디바이스 컴객체
	LPD3DXSPRITE				m_pSprite;		// 스프라이트 컴객체
	LPD3DXFONT					m_pFont;		// 폰트 컴 객체

public:
	static CMyFont*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								const _tchar* pFontType, 
								const _uint& iWidth, 
								const _uint& iHeight, 
								const _uint& iWeight);
	
private:
	virtual void Free(void);
};

