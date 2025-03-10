#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

BEGIN(Client)

class CM_Skeleton_Shield final : public CM_Monster
{
private:
	CM_Skeleton_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Skeleton_Shield(const CM_Skeleton_Shield& rhs);
	virtual ~CM_Skeleton_Shield() = default;

public:
	_bool	Get_Dance() {
		return m_bDance;
	}

public:
	virtual HRESULT		Initialize_Prototype()					override;
	virtual HRESULT		Initialize(void* pArg)					override;
	virtual void		Tick(_double TimeDelta)					override;
	virtual void		Late_Tick(_double TimeDelta)			override;
	virtual HRESULT		Render()								override;
	virtual	HRESULT		Render_XRay() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	virtual HRESULT		SetUp_Components()						override;
	virtual HRESULT		SetUp_ShaderResources()					override;
	HRESULT				SetUp_ShaderXRayResources();

	HRESULT				Ready_Parts();

private:
	virtual void		Monster_Tick(const _double& TimeDelta)	override;
	virtual void		Idle_Tick(const _double& TimeDelta)		override;
	virtual void		Move_Tick(const _double& TimeDelta)		override;
	virtual void		Find_Tick()								override;
	virtual void		Attack_Tick(const _double& TimeDelta)	override;
	virtual void		Hit_Tick(const _double& TimeDelta)		override;
	virtual	void		Die_Tick(const _double& TimeDelta)		override;
	void				Dance_Tick();

	void				Hit_Process(const _double & TimeDelta);
	void				BossCage();

private:
	CShader*			m_pShaderXRayCom = nullptr;

private:
	CGameObject*			m_pOther = { nullptr };
	vector<CGameObject*>	m_MonsterParts;
	_float4					m_f4CenterPos;

	_bool					m_bDance = false;
	_bool					m_bFind = false;
	_bool					m_bAttack = false;

	_int					m_iRandomNum = 0;

	_bool					m_bSword_Sound = false;
	_bool					m_bShield_Sound = false;
	_bool					m_bIdle_Sound = false;

public:
	static	CM_Skeleton_Shield*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free()						override;
};

END