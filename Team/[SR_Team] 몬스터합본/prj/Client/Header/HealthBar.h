#pragma once
#include "UI.h"

class CHealthBar : public CUI
{
private:
	explicit CHealthBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHealthBar();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void);

public:
	static CHealthBar*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	_matrix				m_matProj, m_matView, m_matOrtho, m_matWorld;
	
};

