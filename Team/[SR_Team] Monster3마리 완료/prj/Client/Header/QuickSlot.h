#pragma once
#include "UI.h"

class CQuickSlot : public CUI
{
private:
	explicit CQuickSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CQuickSlot();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void);

public:
	static CQuickSlot*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	_matrix				m_matView, m_matWorld;
	
};

