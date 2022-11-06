#pragma once
#include "UI.h"

class CItemUI : public CUI
{
private:

	explicit CItemUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItemUI();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CItemUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);

	void Close() { m_bClose = true; }
	void Open() { m_bClose = false; }
	_bool IsClosen() { return m_bClose; }
	void SetUITexture(_int TexIndex) { m_iTexNum = TexIndex; }
	void SetPos(const _vec3 vPos) { m_pTransCom->m_vInfo[INFO_POS] = vPos; }
	void SetSize(const _vec3 vSize) { m_pTransCom->m_vScale = vSize; }
	_bool IsPick() { return m_bPick; }
	void SetPick(_bool bPick) { m_bPick = bPick; }

	void SetDelete() { m_bDelete = true;}

private:
	_bool m_bClose = true;
	_bool m_bPick;
	_bool m_bDelete = false;

};

