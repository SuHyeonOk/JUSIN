#pragma once
#include "GameObject.h"

/*------------------------
 * CUI
 ------------------------*/
class CUI : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
	static CUI*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);
	virtual void		Free(void);

	void SetButton(std::function<void()> pCallback) {m_bButtom = true; m_pCallback = pCallback; }
	void SetText(const wstring& wstrFont, const wstring& wstrText, const _vec2& vOffset, const D3DXCOLOR& color)
	{
		m_wstrFont = wstrFont;
		m_wstrText = wstrText;
		m_vTextOffset = vOffset;
		m_TextColor = color;
	}
	void SetDelete() { m_bDelete = true; }

	void SetVisible(_bool bVisible) {m_bVisible = bVisible;}
protected:
	CRcTex*				m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	_uint m_iTexNum = 0;

	_matrix				m_ProjMatrix;

	_bool m_bButtom = false;
	std::function<void()> m_pCallback = nullptr;

	wstring m_wstrFont;
	wstring m_wstrText;
	_vec2 m_vTextOffset;
	D3DXCOLOR m_TextColor;

	_bool m_bDelete = false;
	_bool m_bVisible = true;
};

/*------------------------
 * CCountUI
 ------------------------*/
enum CountUIType
{
	CNT_EMERALD,
	CNT_ARROW,
	CNT_END
};

class CCountUI : public CUI
{
private:
	explicit CCountUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCountUI() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	static CCountUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, CountUIType eType);

private:
	 CountUIType m_eType = CNT_END;
	_uint m_iCnt = 0;
};