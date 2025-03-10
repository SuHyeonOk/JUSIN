#pragma once
#include "Monster.h"

class CController;

class CGeomancer : public CMonster
{
private:
	enum GeomancerState
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
	explicit CGeomancer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGeomancer(const CMonster& rhs);
	virtual ~CGeomancer() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual void AnimationEvent(const string& strEvent) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Free() override;
	static CGeomancer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);

	virtual void StateChange();

	// controller 조종 함수
	void Run(const _vec3& vTargetPos) {m_bMove = true; m_vTargetPos = vTargetPos;}
	void WallSpawn(const vector<_vec3>& vecWallPos)
	{
		m_bAttack = true;
		m_vecWallPos.clear();
		m_vecWallPos.assign(vecWallPos.begin(), vecWallPos.end());
	}
	//

private:
	GeomancerState m_eState = STATE_END;
	array<CubeAnimFrame, ANIM_END> m_arrAnim;

	// true : PlayAnimationOnce 사용 가능 상태(동작 애니메이션 실행 가능), false: 다른 애니메이션 실행중
	_bool m_bCanPlayAnim = true; // 현재 실행중인 애니메이션 끊고 애니메이션 실행 가능 여부

	_vec3 m_vTargetPos = CGameUtilMgr::s_vZero; // controller 입력
	vector<_vec3> m_vecWallPos;

	_bool m_bMove = false; // controller 입력
	_bool m_bAttack = false; // controller 입력

	_bool m_bAttackFire = false; // anim event 입력
};
