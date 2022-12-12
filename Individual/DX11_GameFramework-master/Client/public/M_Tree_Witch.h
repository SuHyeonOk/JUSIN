#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

BEGIN(Client)

class CM_Tree_Wolf final : public CM_Monster
{
private:
	CM_Tree_Wolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Tree_Wolf(const CM_Tree_Wolf& rhs);
	virtual ~CM_Tree_Wolf() = default;

public:
	virtual HRESULT		Initialize_Prototype()					override;
	virtual HRESULT		Initialize(void* pArg)					override;
	virtual void		Tick(_double TimeDelta)					override;
	virtual void		Late_Tick(_double TimeDelta)			override;
	virtual HRESULT		Render()								override;

private:
	virtual HRESULT		SetUp_Components()						override;
	virtual HRESULT		SetUp_ShaderResources()					override;

private:
	virtual void		Monster_Tick(const _double& TimeDelta)	override;
	virtual void		Idle_Tick(const _double& TimeDelta)		override;
	virtual void		Move_Tick(const _double& TimeDelta)		override;
	virtual void		Find_Tick()								override;
	virtual void		Attack_Tick(const _double& TimeDelta)	override;
	virtual void		Hit_Tick()								override;
	virtual	void		Die_Tick()								override;

private:
	virtual void		Collision_ToPlayer()					override;

private:
	_float4				m_f4First_Pos;
	_float4				m_f4RandomPos;
	_bool				m_bRandomPos = false;

	_bool				m_bAttack = false;
	_double				m_dBullet_TimeAcc = 0;

	_int	m_i = 0;

public:
	static	CM_Tree_Wolf*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free()						override;
};

END