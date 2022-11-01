#pragma once
#include "Controller.h"

class CSkeletalCube;
class CPlayer;
class CDynamite;



class CPlayerController : public CController
{
protected:
	explicit CPlayerController(void);
	explicit CPlayerController(const CPlayerController& rhs);
	virtual ~CPlayerController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	static CPlayerController* Create();

protected:
	_vec3 m_vPressDir = CGameUtilMgr::s_vZero;
	_vec3 m_vPrevPressDir = CGameUtilMgr::s_vZero;

	_vec3 m_vMoveDir = CGameUtilMgr::s_vZero;

	void putItem(CPlayer* pPlayer,const  _vec3& vTargetPos);
	void pickGameObj(CPlayer* pPlayer, const  _vec3& vTargetPos);
	CDynamite* m_pDynamite = nullptr;

	_uint m_iPlayerInputMask = 0;
	_uint m_iPreInputMask = 0;

	_float m_fWorldRefreshTime = 1.f;
	_float m_fCurWorldRefreshTime = 1.f;
};

/*----------------------
 * Remote
 -------------------*/
class CPlayerRemoteController : public CPlayerController
{
protected:
	explicit CPlayerRemoteController(void);
	explicit CPlayerRemoteController(const CPlayerRemoteController& rhs);
	virtual ~CPlayerRemoteController() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	static CPlayerRemoteController* Create();

	void SetInputMask(_uint iPlayerInputMask) { m_iPlayerInputMask = iPlayerInputMask; }
	void SetWorld(const _matrix& matWorld)
	{
		if (!m_bWorldSet)
		{
			m_matWorld = matWorld;
			m_bWorldSet.store(true);
		}
	}

	void SetYawAction(_float fYaw, _uint iAction);
	void SetAction(_uint iAction);
	void SetArrow(_float fYaw, _vec3 vLookAt, _uint iAction);

private:
	Atomic<_bool> m_bWorldSet{false};
	_matrix m_matWorld;

	Atomic<_bool> m_bYawActionSet{false};
	Atomic<_uint> m_iYawAction{0};
	_float m_fYaw;

	Atomic<_bool> m_bActionSet{false};
	Atomic<_uint> m_iAction{0};

	Atomic<_bool> m_bArrowActionSet{false};
	Atomic<_uint> m_iArrowAction{0};
	_vec3 m_vLookAt;
};