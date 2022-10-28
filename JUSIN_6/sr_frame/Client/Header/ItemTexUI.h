#pragma once
#include "UI.h"
class CItemTexUI :
	public CUI
{
private:
	explicit CItemTexUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItemTexUI();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CItemTexUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);

public:
	void Close() { m_bClose = true; }
	void Open() { m_bClose = false; }
	_bool IsClosen() { return m_bClose; }
	void SetUITexture(_int TexIndex) { m_iTexNum = TexIndex; }
	void SetRenderPriority(_int iPriority) { m_iRenderPriority = iPriority; }

private:
	_bool m_bClose = true;

	wstring wstrItemName[3]{};
	void SetTexture(_int iTexNum);
	_matrix matworld;
	_bool m_bWorldSet = false;
};

