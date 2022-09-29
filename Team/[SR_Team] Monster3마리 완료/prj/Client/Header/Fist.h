#pragma once
#include "Monster.h"

class CFist : public CMonster
{
	enum STATE { IDLE, ATTACK, HIT, DIE, MOTION_END };

private:
	explicit CFist(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFist();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void) override;
	
	void				Attack_Distance(const _float& fTimeDelta);
	void				Motion_Change(const _float& fTimeDelta);

private:
	STATE				m_ePreState;
	STATE				m_eCurState;

	// Timer
	_float				m_fTimeAcc ;
	_float				m_fIdleTimeAcc;
	_float				m_fAttackTimeAcc;

public:
	static CFist*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};
