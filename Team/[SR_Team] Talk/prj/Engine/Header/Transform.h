#pragma once
#include "Component.h"
#include "Engine_Include.h"

BEGIN(Engine)

// 매 프레임마다 월드행렬을 구성해줄 컴포넌트
class ENGINE_DLL CTransform : public CComponent
{
public:
	explicit CTransform(void);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	const _matrix*	Get_WorldMatrixPointer() { return &m_matWorld; }
	void			Move_Pos(const _vec3* const pDir) { m_vInfo[INFO_POS] += *pDir; }
	void			Rotation(ROTATIONID eID, const _float& fAngle) { *(((_float*)&m_vAngle) + eID) += fAngle; }

	void			Get_Info(INFOID eID, _vec3* pInfo) { memcpy(pInfo, &m_matWorld.m[eID][0], sizeof(_vec3)); }
	void			Set_Info(INFOID eID, _vec3* pInfo) { memcpy(&m_matWorld.m[eID][0], pInfo, sizeof(_vec3)); }


	void			Set_Scale(_float fX, _float fY, _float fZ) { m_vScale = { fX, fY, fZ }; }
	void			Get_Scale(_vec3* _vScale) { _vScale = &m_vScale; }
	void			Set_Pos(const _float& fX, const _float& fY, const _float& fZ) { m_vInfo[INFO_POS] = { fX, fY, fZ }; }
	void			Set_Look(const _float& fX, const _float& fY, const _float& fZ) { m_vInfo[INFO_LOOK] = { fX, fY, fZ }; }

	//void			Set_WorldMatrix(_matrix mat) { m_matWorld = mat; }
	void			Plus_PosY(_float _fY) { m_vInfo[INFO_POS].y += _fY; }
	void            Set_Y(const _float& fY) { m_vInfo[INFO_POS].y = fY; }

	void				Get_WorldMatrix(_matrix* pWorld) const { *pWorld = m_matWorld; }
	void				Set_WorldMatrix(_matrix* pWorld) { m_matWorld = *pWorld; }

public:
	void				Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	void				ChangeHeight_Target(const _vec3* pTargetPos, const _float& fSTest, const _float& fSpeed, const _float& fTimeDelta);
	const _matrix*		Compute_LookAtTarget(const _vec3* pTargetPos);
	// KnockBack
	void				KnockBack_Target(_vec3* pTargetLook, const _float& fSpeed, const _float& fTimeDelta);
	// Jump
	void				Jump(_bool& bJump,
							 const _float& fHeight,
							 _float& fJumpSpeed,
							 const _float& fInitialJumpSpeed,
							 const _float& fAccel,
							 _float fTimeDelta);

public:
	HRESULT			Ready_Transform(void);
	virtual _int	Update_Component(const _float& fTimeDelta);

public:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;

private:
	_bool		m_bJump = false;
	_float		m_fHeight = 0.f;
	_float		m_fJumpSpeed = 0.f;
	_float		m_fInitialJumpSpeed = 0.f;
	_float		m_fAccel = 0.f;	

public:
	static CTransform*	Create(void);
	virtual CComponent* Clone(void);

private:
	virtual void Free(void);
};

END

