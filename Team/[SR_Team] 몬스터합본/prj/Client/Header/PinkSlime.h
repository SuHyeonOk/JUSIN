#pragma once
#include "Monster.h"

BEGIN(Engine)

class CAnimator;

END

class CPinkSlime : public CMonster
{
	enum STATE { IDLE, ATTACK, HIT, DIE, MOTION_END };

private:
	explicit CPinkSlime(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPinkSlime();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	void				Motion_Change(const _float& fTimeDelta);


	virtual HRESULT		Add_Component(void);

	void				Target_Follow(const _float& fTimeDelta);
	void				Billboard();

private:
	STATE				m_ePreState;
	STATE				m_eCurState;

	_float				m_fIdleSpeed;

	_float				m_fTimeAcc;

	// Test
	CAnimator*			m_pAnimater = nullptr;

public:
	static CPinkSlime*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};


