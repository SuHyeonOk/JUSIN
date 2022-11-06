#pragma once
#include "Controller.h"

class CEnchanterController : public CController
{
private:
	explicit CEnchanterController(void);
	explicit CEnchanterController(const CEnchanterController& rhs);
	virtual ~CEnchanterController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CEnchanterController* Create();

private:
	_float m_fCurAttackCoolTime = 0.f;
	_float m_fAttackCoolTime = 1.f;

	_float m_fDetectRange = 6.f; // 사정거리
	_float m_fBuffDiff = 7.f;
	_float m_fAttackDist = 3.f; // 공격 사거리

	
};

