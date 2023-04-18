#pragma once
#include "UI.h"
class CClearUI :
	public CUI
{
private:
	explicit CClearUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CClearUI();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CClearUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);

	void Close() { m_bClose = true; }
	void Open() { m_bClose = false; }
	_bool IsClosen() { return m_bClose; }
	void SetUITexture(_int TexIndex) { m_iTexNum = TexIndex; }

private:
	_bool m_bClose = false;
	_float m_fCount = 5.f;
	wstring m_wstrCount = L"";
};

