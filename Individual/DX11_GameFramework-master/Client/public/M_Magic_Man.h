#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

BEGIN(Client)

class CM_Magic_Man final : public CM_Monster
{
private:
	CM_Magic_Man(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Magic_Man(const CM_Magic_Man& rhs);
	virtual ~CM_Magic_Man() = default;

public:
	virtual HRESULT		Initialize_Prototype()					override;
	virtual HRESULT		Initialize(void* pArg)					override;
	virtual void		Tick(_double TimeDelta)					override;
	virtual void		Late_Tick(_double TimeDelta)			override;
	virtual HRESULT		Render()								override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	virtual HRESULT		SetUp_Components()						override;
	virtual HRESULT		SetUp_ShaderResources()					override;

private:
	virtual void		Monster_Tick(const _double& TimeDelta)	override;
	virtual void		Find_Tick()								override;
	virtual void		Idle_Tick(const _double& TimeDelta)		override;
	virtual void		Move_Tick(const _double& TimeDelta)		override;
	virtual void		Attack_Tick(const _double& TimeDelta)	override;
	virtual void		Hit_Tick(const _double& TimeDelta)		override;
	void				NoHit_Tick();
	virtual	void		Die_Tick(const _double& TimeDelta)		override;

private:
	void				Appear(const _double& TimeDelta);
	void				Hit_Process(const _double & TimeDelta);

private:
	_float4				m_f4CenterPos;

	_bool				m_bFind = false;
	_bool				m_bAttack = false;

	_bool				m_Appear = false;
	_double				m_Appear_TimeAcc = 0;

	_bool				m_bItem = false;

public:
	static	CM_Magic_Man*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free()						override;
};

END