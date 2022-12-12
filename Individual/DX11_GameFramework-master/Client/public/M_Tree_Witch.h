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
	void				Monster_Tick(const _double& TimeDelta);
	void				Idle_Tick(const _double& TimeDelta);
	void				Move_Tick(const _double& TimeDelta);
	void				Find_Tick();
	void				Attack_Tick(const _double& TimeDelta);
	void				Hit_Tick();
	void				Die_Tick();

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