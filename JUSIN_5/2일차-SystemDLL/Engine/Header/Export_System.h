#ifndef Export_System_h__
#define Export_System_h__

#include "Engine_Include.h"
#include "GraphicDev.h"
#include "TimerMgr.h"

BEGIN(Engine)

// GraphicDev
inline HRESULT		Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev);
inline void			Render_Begin(D3DXCOLOR Color);
inline void			Render_End(HWND hWnd = nullptr);


// TimerMgr
inline _float		Get_TimeDelta(const _tchar* pTimerTag);
inline HRESULT		Ready_Timer(const _tchar* pTimerTag);
inline void			Update_Timer(const _tchar* pTimerTag);

inline void			Release_System(void); // ΩÃ±€≈Ê¿« ∆ƒ±•µµ ø©±‚º≠ »£√‚ µ«µµ∑œ


#include "Export_System.inl"

END
#endif // Export_System_h__
