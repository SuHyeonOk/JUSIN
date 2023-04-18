#pragma once
#include "Controller.h"

class CZombieController : public CController
{
protected:
	explicit CZombieController(void);
	explicit CZombieController(const CZombieController& rhs);
	virtual ~CZombieController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
public:
	static CZombieController* Create();

protected:
	_float m_fCurAttackCoolTime = 0.f;
	_float m_fAttackCoolTime = 2.f;

	_float m_fDetectRange = 14.f; // 사정거리
	_float m_fAttackDist = 3.f; // 공격 사거리

	_float m_fWorldRefreshTime = 1.f;
	_float m_fCurWorldRefreshTime = 1.f;
};

class CZombieRemoteController : public CZombieController
{
protected:
	explicit CZombieRemoteController(void);
	explicit CZombieRemoteController(const CZombieRemoteController& rhs);
	virtual ~CZombieRemoteController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	static CZombieRemoteController* Create();

	void SetTarget(_uint iTargetID);
	void SetAttack()
	{
		m_bAttackSet.store(true);
	}
	void SetWorld(const _matrix& matWorld)
	{
		m_matWorld = matWorld;
		m_bWorldSet.store(true);
	}

private:
	Atomic<_bool> m_bWorldSet{false};
	_matrix m_matWorld;

	_vec3 m_vTargetPos;

	Atomic<_bool> m_bTargetSet{false};
	_uint m_iTargetID = 0;

	Atomic<_bool> m_bAttackSet{false};
};