#pragma once
#include "Controller.h"
class CLeaperController :
	public CController
{
private:
	explicit CLeaperController(void);
	explicit CLeaperController(const CLeaperController& rhs);
	virtual ~CLeaperController();

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CLeaperController* Create();

private:
	_float m_fAttakCollTime = 2.f;
	_float m_fCurAttackCollTime = 0.f;

	_float m_fJumpCollTime = 8.f;
	_float m_fCurJumpCollTime = 0.f;
};

