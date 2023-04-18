#pragma once
#include "Controller.h"

class CRedStoneCubeController : public CController
{
private:
	explicit CRedStoneCubeController(void);
	explicit CRedStoneCubeController(const CRedStoneCubeController& rhs);
	virtual ~CRedStoneCubeController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CRedStoneCubeController* Create();

private:
	_float m_fDetectRange = 20.f; // 사정거리
	_float m_fAttackDist = 2.f; // 공격 사거리
};

