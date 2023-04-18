#pragma once
#include "Monster.h"

class CBossHPUI;

class CEnderman :
	public CMonster
{
private:
	enum CEndermanState
	{
		IDLE,
		WALK,
		CHOP,
		SMASH,
		CLOCKING,
		ARMATTACK,
		PISTOL,
		STUN,
		DEAD,
		STATE_END
	};

	enum Animation
	{
		ANIM_WALK,
		ANIM_IDLE,
		ANIM_DEAD,
		ANIM_CHOP,
		ANIM_SMASH,
		ANIM_ARMATTACK,
		ANIM_PISTOL,
		ANIM_END
	};

private:
	explicit CEnderman(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEnderman(const CMonster& rhs);
	virtual ~CEnderman();

public:
	virtual HRESULT Ready_Object() override;
	virtual void AnimationEvent(const string& strEvent) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Free() override;
	static CEnderman* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote = false);

	virtual void StateChange();

	// controller 조종 함수
	void WalkToTarget() { m_bMove = true;}
	void DoChop() { m_bChop = true; }
	void DoSmash() { m_bSmash = true; }
	void DoIdle() { m_bMove = false; }
	void DoArmAttack() { m_bArmAttack = true; }
	void DoPistol() { m_bPistol = true; }
	void DoClocking(_vec3 vLook)
	{
		m_bClocking = true;
		m_fCurClockingCollTime = 0.f;
		m_vBeforTargetLook = vLook;
	}
	void SetTarget(_vec3 vTargetPos){ m_vTargetPos = vTargetPos; }
	void CreateTrail(const vector<string>& vecExcludeParts);
	//

private:
	CEndermanState m_eState = STATE_END;
	array<CubeAnimFrame, ANIM_END> m_arrAnim;

	// true : PlayAnimationOnce 사용 가능 상태(동작 애니메이션 실행 가능), false: 다른 애니메이션 실행중
	_bool m_bCanPlayAnim = true; // 현재 실행중인 애니메이션 끊고 애니메이션 실행 가능 여부

	_vec3 m_vTargetPos = CGameUtilMgr::s_vZero; // controller 입력

	_bool m_bMove = false; // controller 입력
	_bool m_bChop = false; // controller 입력
	_bool m_bSmash = false; // controller 입력
	_bool m_bAttackFire = false; // anim event 입력
	_bool m_bClocking = false; // anim event 입력
	_bool m_bArmAttack = false; // anim event 입력
	_bool m_bArmAttakcFire = false;

	_float m_fClockingCollTime = 2.5f;
	_float m_fCurClockingCollTime = 0.f;

	_vec3 m_vBeforTargetLook;
	CBossHPUI* m_pBossHPUI = nullptr;

	_bool m_bPistol = false;
	_bool m_bPistolStart = false;
	_float m_fPistolFreq = 0.2f;
	_float m_fCurPistolTime = 0.4f;

	_bool m_bCreateTrail = false;
};

