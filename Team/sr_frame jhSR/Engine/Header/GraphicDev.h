#pragma once

#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CGraphicDev : public CBase
{
	DECLARE_SINGLETON(CGraphicDev)

public:
	explicit CGraphicDev();
	virtual ~CGraphicDev();

public:
	LPDIRECT3DDEVICE9		Get_GraphicDev(void) { return m_pGraphicDev; }
	LPD3DXFONT				GetFont() { return m_pFont; }

public:
	HRESULT			Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev);
	void			Render_Begin(D3DXCOLOR Color);
	void			Render_End(HWND hWnd = nullptr);
private:


private:
	LPDIRECT3D9			m_pSDK;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LPD3DXFONT m_pFont;

public:
	virtual void	Free(void);
};
END