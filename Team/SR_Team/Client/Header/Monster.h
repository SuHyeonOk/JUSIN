#pragma once
#include "GameObject.h"


class CMonster : public CGameObject
{
protected:
	enum STATE { IDLE, ATTACK, HIT, DIE, MOTION_END };

public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual HRESULT		Ready_Object(void)						override { return S_OK; };
	virtual _int		Update_Object(const _float& fTimeDelta) override { return 0; };
	virtual void		LateUpdate_Object(void)					override {};
	virtual void		Render_Obejct(void)						override {};

public:
	// Get / Set

protected:
	virtual HRESULT		Add_Component(void) PURE;

protected:
	_float				m_fTimeDelta;

	_float				m_fFrame;

	STATE				m_ePreState;
	STATE				m_eCurState;

	_float				m_fSpeed;

public:
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev) {};
	virtual void		Free(void);
};

