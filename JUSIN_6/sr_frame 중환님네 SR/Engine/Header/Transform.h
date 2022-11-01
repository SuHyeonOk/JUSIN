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
	const _matrix* Get_WorldMatrixPointer() { return &m_matWorld; }
	void	Move_Pos(const _vec3* const pDir) { m_vInfo[INFO_POS] += *pDir; }
	void	Rotation(ROTATIONID eID, const _float& fAngle) { *(((_float*)&m_vAngle) + eID) += fAngle; }
	void	Get_Info(INFOID eID, _vec3* pInfo) { memcpy(pInfo, &m_matWorld.m[eID][0], sizeof(_vec3)); }
	void	Set_Scale(_float fX, _float fY, _float fZ) { m_vScale = { fX, fY, fZ }; }
	void	Set_Pos(const _float& fX, const _float& fY, const _float& fZ)
	{
		m_vInfo[INFO_POS].x = fX;
		m_vInfo[INFO_POS].y = fY;
		m_vInfo[INFO_POS].z = fZ;
	}
	void	Set_ScaleY(const _float& fY) { m_vScale.y = fY; }
	void	SetRotation(const _vec3& vAngle) { m_vAngle = vAngle; }
	void	Get_WorldMatrix(_matrix* pWorld) const { *pWorld = m_matWorld; }
	void	Set_WorldMatrix(_matrix* pWorld) { m_matWorld = *pWorld; }
	void	Set_WorldMatrix(const _matrix* pWorld) { m_matWorld = *pWorld; }
	void    Set_WorldDecompose(const _matrix& matWorld);
	void    Set_StopUpdate() { m_bStopUpdate = true; }
	void    Set_StartUpdate() { m_bStopUpdate = false; }

public:
	void Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	const _matrix* Compute_LookAtTarget(const _vec3* pTargetPos);
	void RotateToLookAt(const _vec3& vLookAt);



public:
	HRESULT Ready_Transform(void);
	virtual _int Update_Component(const _float& fTimeDelta);


public:
	_vec3 m_vInfo[INFO_END];
	_vec3 m_vScale;
	_vec3 m_vAngle;
	_matrix m_matWorld;
	_float m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool m_bStopUpdate = false;


public:
	static CTransform* Create(void);
	virtual CComponent* Clone(void) override;
private:
	virtual void Free(void) override;
};

END
