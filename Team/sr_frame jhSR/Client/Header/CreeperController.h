#pragma once
#include "Controller.h"

class CCreeperController : public CController
{
private:
	explicit CCreeperController(void);
	explicit CCreeperController(const CCreeperController& rhs);
	virtual ~CCreeperController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CCreeperController* Create();

private:
	_float m_fCountStartDist = 14.f; // 폭발 카운트 시작 거리
	_float m_fExplosionCount = 0.f;

	_bool m_bWalk = false;
};

