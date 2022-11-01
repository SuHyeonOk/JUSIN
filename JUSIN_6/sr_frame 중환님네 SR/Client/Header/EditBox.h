#pragma once
#include "UI.h"
class CEditBox : public CUI
{
private:
	explicit CEditBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEditBox() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	static CEditBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	string GetInputs() const { return m_strInputs; }
private:
	string m_strInputs;
};

