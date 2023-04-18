#pragma once

#include "SkeletalCube.h"

class CCat2 : public CSkeletalCube
{
private:
	explicit CCat2(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCat2(const CCat2& rhs);
	virtual ~CCat2() override;

	enum CCat2State
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
	virtual HRESULT	Ready_Object() override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object() override;
	virtual void	Free() override;
	static CCat2*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);

	virtual void	StateChange();

	void			WalkToTarget(const _vec3& vTragetPos) { m_bMove = true; m_vTargetPos = vTragetPos; }
	void			RotateToTargetPos(const _vec3& vTargetPos, bool bReverse = false);

private:
	CCat2State m_eState = STATE_END;
	array<CubeAnimFrame, ANIM_END> m_arrAnim;

	_float	m_fSpeed = 0.f;

	_bool m_bCanPlayAnim = true;

	_bool m_bMove = false;
	// _vec3 m_vTargetPos = CGameUtilMgr::s_vZero;
	_vec3 m_vTargetPos = { 22.f,0.f,80.f };

	_float		m_fBoxRange = 10.f;
	_float		m_fMoveDist = 2.f;

	CTerrainCubeMap* m_pCubeMap = nullptr;
};

