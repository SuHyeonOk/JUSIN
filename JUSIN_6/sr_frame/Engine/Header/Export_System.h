#ifndef Export_System_h__
#define Export_System_h__

#include "Engine_Include.h"
#include "GraphicDev.h"
#include "TimerMgr.h"
#include "FrameMgr.h"
// #include "InputMgr.h"
#include "InputDev.h"
#include "FontMgr.h"

BEGIN(Engine)

// GraphicDev
inline HRESULT		Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev);
inline void			Render_Begin(D3DXCOLOR Color);
inline void			Render_End(HWND hWnd = nullptr);
inline LPD3DXFONT	GetFont();

// TimerMgr
inline _float		Get_TimeDelta(const wstring& pTimerTag);
inline HRESULT		Ready_Timer(const wstring& pTimerTag);
inline void			Update_Timer(const wstring& pTimerTag);

// FrameMgr
inline _bool		IsPermit_Call(const wstring& pFrameTag, const _float& fTimeDelta);
inline HRESULT		Ready_Frame(const wstring& pFrameTag, const _float& fCallLimit);

// InputDev
inline _byte		Get_DIKeyState(_ubyte byKeyID);
inline _byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID);
inline _long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
inline	HRESULT		Ready_InputDev(HINSTANCE hInst, HWND hWnd);
inline void			SetUp_InputDev(void);
inline bool DIKeyPressing(_ubyte byKeyID);
inline bool DIKeyDown(_ubyte byKeyID);
inline bool DIKeyUp(_ubyte byKeyID);
inline bool MouseKeyDown(MOUSEKEYSTATE byMouseID);
inline bool MouseKeyUp(MOUSEKEYSTATE byMouseID);
inline bool MouseKeyPress(MOUSEKEYSTATE byMouseID);

// FontMgr

inline HRESULT			Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const wstring& pFontTag,	 // 키값
	const _tchar* pFontType, // 서체
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight);

inline void	Render_Font(const wstring& pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color);


inline void			Release_System(void);


#include "Export_System.inl"

END
#endif // Export_System_h__
