#pragma once

#include "SkeletalCube.h"

class CCat : public CSkeletalCube
{
private:
	explicit CCat(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCat(const CCat& rhs);
	virtual ~CCat() override;

	enum CCatState
	{
		IDLE,
		WALK,
		STATE_END
	};

	enum Animation
	{
		ANIM_WALK,
		ANIM_IDLE,
		ANIM_END
	};

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Free() override;
	static CCat* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);

	virtual void StateChange();

	void	WalkToTarger(const _vec3& vTargetPos) { m_bMove = true; m_vTargetPos = vTargetPos; }

	void	RotateToTargetPos(const _vec3& vTargetPos, bool bReverse = false);

private:

	CCatState m_eState = STATE_END;
	array<CubeAnimFrame, ANIM_END> m_arrAnim;

	_float		m_fSpeed = 0.f;
	// true : PlayAnimationOnce 사용 가능 상태(동작 애니메이션 실행 가능), false: 다른 애니메이션 실행중

	// anim event 입력
	_bool m_bCanPlayAnim = true; // 현재 실행중인 애니메이션 끊고 애니메이션 실행 가능 여부

	_bool m_bMove = false;
	_vec3 m_vTargetPos = CGameUtilMgr::s_vZero; // controller 입력
	
	_float		m_fPlayerRange = 10.f;
	_float		m_fMoveDist = 2.f;

	_vec3 vEndPos = { 5.f,9.f,27.f };
	_bool m_bGo = true;

	CTerrainCubeMap* m_pCubeMap = nullptr;

};

