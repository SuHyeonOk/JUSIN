#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"
#include "Skill_Manager.h"

BEGIN(Client)

class CM_Penny final : public CM_Monster
{
private:
	CM_Penny(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Penny(const CM_Penny& rhs);
	virtual ~CM_Penny() = default;

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
	virtual	void		Die_Tick(const _double& TimeDelta)		override;

	void				PennyCopy_Create();

private:
	_float4				m_f4CenterPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	CSkill_Manager::PLAYERSKILL::SKILL	m_ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL_END;

public:
	static	CM_Penny*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free()						override;
};

END