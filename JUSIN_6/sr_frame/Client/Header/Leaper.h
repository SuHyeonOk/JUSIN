#pragma once
#include "Monster.h"
class CLeaper :
	public CMonster
{
private:

	enum class State : _int
	{
		IDLE,
		WALK,
		CHOP,
		ATTACK,
		JUMP,
		STUN,
		DEAD,
		STATE_END
	};

	enum Animation
	{
		ANIM_WALK,
		ANIM_IDLE,
		ANIM_DEAD,
		ANIM_ATTACK,
		ANIM_JUMP,
		ANIM_END
	};
	explicit CLeaper(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CLeaper(const CMonster& rhs);
	virtual ~CLeaper();

public:
	virtual HRESULT Ready_Object() override;
	virtual void AnimationEvent(const string& strEvent) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Free() override;
	static CLeaper* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote = false);

	virtual void StateChange();

	// controller 조종 함수
	void WalkToTarget() { m_bMove = true; }
	void DoAttack() { m_bAttack = true; }
	void DoJump() { m_bJump = true; }
	void DoIdle() { m_bMove = false; }
	void SetTarget(_vec3 vTargetPos) { m_vTargetPos = vTargetPos; }

	//

private:
	State m_eState = State::STATE_END;
	array<CubeAnimFrame, ANIM_END> m_arrAnim;

	// true : PlayAnimationOnce 사용 가능 상태(동작 애니메이션 실행 가능), false: 다른 애니메이션 실행중
	_bool m_bCanPlayAnim = true; // 현재 실행중인 애니메이션 끊고 애니메이션 실행 가능 여부

	_vec3 m_vTargetPos = CGameUtilMgr::s_vZero; // controller 입력

	_bool m_bMove = false; // controller 입력
	_bool m_bAttack = false; // controller 입력
	_bool m_bJump = false; // controller 입력
	_bool m_bAttackFire = false; // anim event 입력

	void JumpToPlayer(const _float& fTimeDelta);
	_float m_fJumpTime = 0.1f;
	_bool m_bLanding = true;
	_vec3 m_vJumpTargetPos;
	_bool m_bJumpFire = false;

};

