#pragma once
#include "GameObject.h"


class CMonster : public CGameObject
{
	typedef	struct	tagFrame
	{
		int		iFrameStart;
		int		iFrameEnd;
	}FRAME;

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

	FRAME		m_tFrame;

	_vec3				m_vDirection;
	_float				m_fTimeDelta;

	float				m_fFrame;

public:
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev) {};
	virtual void		Free(void);
};

