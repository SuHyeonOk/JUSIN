#pragma once
#include "Controller.h"

class CPlayer;

class CRedStoneMonstrosityController : public CController
{
private:
	explicit CRedStoneMonstrosityController(void);
	explicit CRedStoneMonstrosityController(const CRedStoneMonstrosityController& rhs);
	virtual ~CRedStoneMonstrosityController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CRedStoneMonstrosityController* Create();

private:
	CPlayer* m_pTargetPlayer = nullptr;



	_float m_fCurChopCoolTime = 0.f;
	_float m_fCurSpitCoolTime = 0.f;
	_float m_fCurSummonCoolTime = 0.f;
	_float m_fCurWindmillCoolTime = 0.f;

	_float m_fChopCoolTime = 15.f;
	_float m_fSpitCoolTime = 13.f;
	_float m_fSummonCoolTime = 22.f;
	/*_float m_fWindmillCoolTime = 25.f;*/
	_float m_fWindmillCoolTime = 27.f;

	_float m_fChopDist = 4.f;
	_float m_fSpitDist = 10.f;
	_float m_fWindmillDist = 5.f;
};

