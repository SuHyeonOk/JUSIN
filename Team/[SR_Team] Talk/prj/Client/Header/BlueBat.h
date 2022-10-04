#pragma once
#include "Monster.h"

BEGIN(Engine)

class CCalculator;

class CBlueBat : public CMonster
{
	enum STATE { IDLE, ATTACK, HIT, DIE, MOTION_END };

private:
	explicit CBlueBat(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBlueBat();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void) override;
	virtual void		Target_Follow(const _float& fTimeDelta);

	void				Jump(const _float& fTimeDelta);
	void				KnockBack(const _float& fTimeDelta);

	void				Motion_Change(const _float& fTimeDelta);

private:
	STATE				m_ePreState;
	STATE				m_eCurState;

	_bool				m_bIdle;

	// 점프 관련 변수
	_bool				m_bJump = false;
	_float				m_fHeight = 1.f;
	_float				m_fJSpeed; 			// 점프 속도
	_float				m_fJSpeed0; 		// 점프 초기 속도
	_float				m_fAccel;			// 중력가속도

	// 넉백 관련 변수
	_bool				m_bKnockBack = false;
	_float				m_fBSpeed = 0.1f;
	_float				m_fBSpeed0 = 0.1f;
	_float				m_fBAccel = 0.01f;
	_float				m_fKnockBackSpeed;

	//Timer
	_float				m_fTimeAcc;
	_float				m_fJumpTimeAcc;
	_float				m_fIdleTimeAcc;
	_float				m_fSkillTimeAcc;
	_float				m_fBTimeDelta = 0.f;

public:
	static CBlueBat*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};

END