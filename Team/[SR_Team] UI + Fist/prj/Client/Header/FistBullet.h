#pragma once
#include "Bullet.h"

BEGIN(Engine)

class CAnimator;

class CFistBullet : public CBullet
{
private:
	explicit CFistBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFistBullet(const CFistBullet& rhs);
	virtual ~CFistBullet();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual	void	Render_Obejct(void) override;

public:
	static CFistBullet*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void) override;
	virtual void			Reset() override;

private:
	HRESULT					Add_Component(void);

private:
	CAnimator*				m_pAnumtorCom = nullptr;

private:
	_float		m_fFrame = 0.f;
	_float		m_fSpeed = 10.f;

	_bool		m_bReady = false;
	_vec3		m_vDirection = { 0.f,0.f,0.f };
};

END