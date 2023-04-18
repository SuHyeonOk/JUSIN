#pragma once
#include "Controller.h"

class CSkeletonController : public CController
{
private:
	explicit CSkeletonController(void);
	explicit CSkeletonController(const CSkeletonController& rhs);
	virtual ~CSkeletonController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CSkeletonController* Create();

private:
	_float m_fCurAttackCoolTime = 0.f;
	_float m_fAttackCoolTime = 3.f;

	_float m_fAttackRange = 18.f; // 사정거리
	_float m_fRunDist = 6.f;
};

