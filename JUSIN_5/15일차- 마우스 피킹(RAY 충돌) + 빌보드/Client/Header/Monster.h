#pragma once
#include "GameObject.h"


class CMonster : public CGameObject
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual HRESULT		Ready_Object(void)						override { return S_OK; };
	virtual _int		Update_Object(const _float& fTimeDelta) override { return 0; };
	virtual void		LateUpdate_Object(void)					override {};
	virtual void		Render_Obejct(void)						override {};

public:
	// GEt/ set

private:
	virtual HRESULT		Add_Component(void) PURE;

private:
	_vec3				m_vDirection;
	_float				m_fTimeDelta = 0.f;

public:
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev) {};
	virtual void		Free(void);
};

