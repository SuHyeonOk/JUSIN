#pragma once
#include "UI.h"
class CPlayerUI :
	public CUI
{
private:
	CPlayerUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerUI();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CPlayerUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);

	void Close() { m_bClose = true; }
	void Open() { m_bClose = false; }
	_bool IsClosen() { return m_bClose; }
	void SetUITexture(_int TexIndex) { m_iTexNum = TexIndex; }
	void SetNoCount(){m_bCount = false;};
	void SetCount(_float fCnt) { m_fCount = fCnt;}
private:
	_bool m_bClose = true;
	_float m_fCount = 3.f;
	wstring m_wstrCount = L"";
	_bool m_bCount = true;
};

