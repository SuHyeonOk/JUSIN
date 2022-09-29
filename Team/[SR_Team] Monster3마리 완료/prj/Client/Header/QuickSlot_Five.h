#pragma once
#include "UI.h"

class CQuickSlot_Five : public CUI
{
private:
	explicit CQuickSlot_Five(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CQuickSlot_Five();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void);

public:
	static CQuickSlot_Five*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	_matrix				m_matView, m_matWorld;
	
};

