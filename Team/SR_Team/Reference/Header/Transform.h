#pragma once
#include "Component.h"
#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
public:
	explicit CTransform(void);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	const _matrix*		Get_WorldMatrixPointer()					 {	return &m_matWorld; }
	void				Move_Pos(const _vec3* const pDir)			 {	m_vInfo[INFO_POS] += *pDir; }
	void				Rotation(ROTATIONID eID, const _float& fAngle){	*(((_float*)&m_vAngle) + eID) += fAngle;	}
	void				Get_Info(INFOID eID, _vec3* pInfo) {	memcpy(pInfo, &m_matWorld.m[eID][0], sizeof(_vec3));}
	void				Set_Scale(_float fX, _float fY, _float fZ){ 	m_vScale = { fX, fY, fZ };	}
	void				Set_Pos(const _float& fX, const _float& fY, const _float& fZ) 
	{
		m_vInfo[INFO_POS].x = fX;
		m_vInfo[INFO_POS].y = fY;
		m_vInfo[INFO_POS].z = fZ;
	}
	void				Set_X(const _float& fX) { m_vInfo[INFO_POS].x = fX; }
	void				Set_Y(const _float& fY) { m_vInfo[INFO_POS].y = fY; }

	void				Get_WorldMatrix(_matrix* pWorld) const { *pWorld = m_matWorld; }
	void				Set_WorldMatrix(_matrix* pWorld) {  m_matWorld = *pWorld; }

public:
	void				Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	const _matrix*		Compute_LookAtTarget(const _vec3* pTargetPos);

public:
	HRESULT				Ready_Transform(void);
	virtual _int		Update_Component(const _float& fTimeDelta);

public:
	_vec3			m_vInfo[INFO_END];
	_vec3			m_vScale;
	_vec3			m_vAngle;
	_matrix			m_matWorld;

public:
	static CTransform*		Create(void);
	virtual CComponent*	Clone(void);
private:
	virtual void Free(void);

};

END