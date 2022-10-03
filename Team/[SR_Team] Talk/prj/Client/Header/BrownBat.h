#pragma once
#include "Monster.h"

class CBrownBat : public CMonster
{
	enum STATE { IDLE, ATTACK, HIT, DIE, MOTION_END };

private:
	explicit CBrownBat(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBrownBat();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void) override;
	virtual void		Target_Follow(const _float& fTimeDelta);

	void				Motion_Change(const _float& fTimeDelta);

private:
	STATE				m_ePreState;
	STATE				m_eCurState;

	_float				m_fHeight;


	// Timer
	_float				m_fTimeAcc;
	_float				m_fAttackTimeAcc;

public:
	static CBrownBat*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};


