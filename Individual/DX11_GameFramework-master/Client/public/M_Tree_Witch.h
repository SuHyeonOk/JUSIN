#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

#include "Skill_Manager.h"

BEGIN(Client)

class CM_Tree_Witch final : public CM_Monster
{
public:
	enum SKILLANIMSTATE { JUMP, PRESSURE, RISE, STATE_END };

private:
	CM_Tree_Witch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Tree_Witch(const CM_Tree_Witch& rhs);
	virtual ~CM_Tree_Witch() = default;

public:
	SKILLANIMSTATE		Get_MonsterSkill() { return m_eSkill_AnimState; }

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
	virtual void		Idle_Tick(const _double& TimeDelta)		override;
	virtual void		Move_Tick(const _double& TimeDelta)		override;
	virtual void		Find_Tick()								override;
	virtual void		Attack_Tick(const _double& TimeDelta)	override;
	void				Attack_Tick2(const _double& TimeDelta);
	virtual void		Hit_Tick(const _double& TimeDelta)		override;
	virtual	void		Die_Tick(const _double& TimeDelta)		override;

private:
	_float4				m_f4PlayerPos;

	_int				m_iRandomNum = 0;

	SKILLANIMSTATE		m_eSkill_AnimState;
	_bool				m_bAttack = false;
	_double				m_dBullet_TimeAcc = 0;

	_bool				m_bAttack_Sound = false;

public:
	static	CM_Tree_Witch*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free()						override;
};

END