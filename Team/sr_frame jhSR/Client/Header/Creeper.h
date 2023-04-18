#pragma once
#include "Monster.h"

class CController;

class CCreeper : public CMonster
{
private:
	enum CCreeperState
	{
		IDLE,
		WALK,
		ATTACK,
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
		ANIM_END
	};

private:
	explicit CCreeper(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCreeper(const CMonster& rhs);
	virtual ~CCreeper() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual void AnimationEvent(const string& strEvent) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CCreeper* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);

	virtual void StateChange();

	// controller 조종 함수
	void WalkToTarget(const _vec3& vTargetPos) { m_bMove = true; m_vTargetPos = vTargetPos; }
	void AttackCountStart() { m_bAttackCount = true; CSoundMgr::GetInstance()->PlaySound(L"sfx_multi_creeperFuse-001_soundWave.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.7f);  }
	void Explosion(const _vec3& vTargetPos)
	{
		m_bAttack = true;
		m_bMove = false;
		m_vTargetPos = vTargetPos;
	}
	//

private:
	CCreeperState m_eState = STATE_END;
	array<CubeAnimFrame, ANIM_END> m_arrAnim;

	// true : PlayAnimationOnce 사용 가능 상태(동작 애니메이션 실행 가능), false: 다른 애니메이션 실행중

	// anim event 입력
	_bool m_bAttackFire = false; 
	_bool m_bCanPlayAnim = true; // 현재 실행중인 애니메이션 끊고 애니메이션 실행 가능 여부

	// controller 입력
	_bool m_bMove = false; 
	_bool m_bAttack = false; 
	_bool m_bAttackCount = false; 
	_bool m_bTwinkle = false;
	_vec3 m_vTargetPos = CGameUtilMgr::s_vZero;

	_float m_fFireShaderCount = 0.3f;
	_float m_fCurFireShaderCount = 1.f;
	_float m_fFreq = 1.f;
};
