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

	void				Motion_Change(const _float& fTimeDelta);

	void				Jump(const _float& fTimeDelta);
	//float				Get_Height();

private:
	STATE				m_ePreState;
	STATE				m_eCurState;

	_bool				m_bIdle = false;

	_int m_iFrame = 0.f;

	// 점프 관련 변수
	_bool				m_bJump = false;
	_float				m_fHeight = 1.f;
	_float				m_fJSpeed ; 		// 점프 속도
	_float				m_fJSpeed0; 		// 점프 초기 속도
	_float				m_fAccel;			// 중력가속도

	//Timer
	_float				m_fTimeAcc;
	_float				m_fJumpTimeAcc;
	_float				m_fIdleTimeAcc = 0.f;
	_float				m_fSkillTimeAcc = 0.f;
	_float				m_CameraShakeTimeAcc = 0.f;


	// 점프 관련 변수

	CCalculator*		m_pCalculatorCom = nullptr;

	//_vec3				m_vDirection;
	//_float				m_fSpeed = 5.f;
	//_float				m_fScale = 1.f;

	//_bool				m_bJump = false;
	//_float				m_fJSpeed; 		// 점프 속도
	//_float				m_fJSpeed0; 	// 점프 초기 속도
	//_float				m_fAccel;		// 중력가속도

public:
	static CBlueBat*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};

END