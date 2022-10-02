#pragma once
#include "UI.h"

class CTalkWindow : public CUI
{
private:
	explicit CTalkWindow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTalkWindow();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

public:
	void				Set_Text(_bool _bTest) { m_bText = _bTest; }


private:
	virtual HRESULT		Add_Component(void);
	void				TextSetting();

private:

public:
	static CTalkWindow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	_matrix				m_matView, m_matWorld;
	_float				m_fTempPosX, m_fTempPosY = 2022.f;	

private:
	_tchar				m_szNPCText[128];
	_uint				m_iTextCount;
	_bool				m_bText = false;

};

