#pragma once
#include "UI.h"

class CHPGauge : public CUI
{
private:
	explicit CHPGauge(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHPGauge();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void);

public:
	static CHPGauge*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	_matrix				m_matView, m_matWorld/*, m_matOrtho*/;

private:
	_float				m_fMinusHp;
	_float				m_fHp;
	_float				m_fMaxHp, m_fMinHp;

	// Timer
	_float				m_fLBClick;
	
};

