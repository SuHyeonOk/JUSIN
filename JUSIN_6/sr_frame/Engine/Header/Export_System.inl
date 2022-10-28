/*--------------------
 *		CGraphicDev
 --------------------*/
HRESULT		Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev)
{
	return CGraphicDev::GetInstance()->Ready_GraphicDev(hWnd, eMode, iSizeX, iSizeY, ppGraphicDev);
}
void			Render_Begin(D3DXCOLOR Color)
{
	CGraphicDev::GetInstance()->Render_Begin(Color);
}
void			Render_End(HWND hWnd)
{
	CGraphicDev::GetInstance()->Render_End(hWnd);
}
LPD3DXFONT	GetFont()
{
	return CGraphicDev::GetInstance()->GetFont();
}

/*--------------------
 *		CTimerMgr
 --------------------*/
_float		Get_TimeDelta(const wstring& pTimerTag)
{
	return CTimerMgr::GetInstance()->Get_TimeDelta(pTimerTag);
}
HRESULT		Ready_Timer(const wstring& pTimerTag)
{
	return CTimerMgr::GetInstance()->Ready_Timer(pTimerTag);
}
void			Update_Timer(const wstring& pTimerTag)
{
	CTimerMgr::GetInstance()->Update_Timer(pTimerTag);
}

_bool		IsPermit_Call(const wstring& pFrameTag, const _float& fTimeDelta)
{
	return CFrameMgr::GetInstance()->IsPermit_Call(pFrameTag, fTimeDelta);
}
HRESULT		Ready_Frame(const wstring& pFrameTag, const _float& fCallLimit)
{
	return CFrameMgr::GetInstance()->Ready_Frame(pFrameTag, fCallLimit);
}

/*--------------------
 *		InputDev
 --------------------*/
_byte		Get_DIKeyState(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Get_DIKeyState(byKeyID);
}
_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID)
{
	return CInputDev::GetInstance()->Get_DIMouseState(byMouseID);
}
 
_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
{
	return CInputDev::GetInstance()->Get_DIMouseMove(eMoveState);
}
HRESULT		Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	return CInputDev::GetInstance()->Ready_InputDev(hInst, hWnd);
}
void			SetUp_InputDev(void)
{
	CInputDev::GetInstance()->SetUp_InputDev();
 }

inline bool DIKeyPressing(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->DIKeyPressing(byKeyID);
}

inline bool DIKeyDown(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->DIKeyDown(byKeyID);
}

inline bool DIKeyUp(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->DIKeyUp(byKeyID);
}

inline bool MouseKeyDown(MOUSEKEYSTATE byMouseID)
{
	return CInputDev::GetInstance()->MouseKeyDown(byMouseID);
}

inline bool MouseKeyUp(MOUSEKEYSTATE byMouseID)
{
	return CInputDev::GetInstance()->MouseKeyUp(byMouseID);
}
inline bool MouseKeyPress(MOUSEKEYSTATE byMouseID)
{
	return CInputDev::GetInstance()->MouseKeyPress(byMouseID);
}

/*--------------------
 *		FontMgr
 --------------------*/
HRESULT			Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const wstring& pFontTag,	 // 키값
	const _tchar* pFontType, // 서체
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	return CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pFontTag, pFontType, iWidth, iHeight, iWeight);
}

void	Render_Font(const wstring& pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color)
{
	CFontMgr::GetInstance()->Render_Font(pFontTag, pString, pPos, Color);
}

inline void			Release_System(void)
{
	CFontMgr::GetInstance()->DestroyInstance();
	CInputDev::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
}
