#pragma once
#include "Bullet.h"

BEGIN(Engine)

class CRcTex;
class CAnimator;

class CSongBossBullet : public CBullet
{
private:
	explicit CSongBossBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSongBossBullet(const CSongBossBullet& rhs);
	virtual ~CSongBossBullet();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual	void	Render_Obejct(void) override;

public:
	static CSongBossBullet*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void			Free(void) override;
	virtual void			Reset() override;

private:
	void					Billboard();

private:
	HRESULT					Add_Component(void);

private:
	CRcTex*					m_pBufferCom = nullptr;
	CAnimator*				m_pAnimtorCom = nullptr;

private:
	_float		m_fFrame = 0.f;
	_float		m_fSpeed;

	_bool		m_bReady = false;
	_vec3		vPos, m_vPlayerPos = { 0.f,0.f,0.f };

};

END