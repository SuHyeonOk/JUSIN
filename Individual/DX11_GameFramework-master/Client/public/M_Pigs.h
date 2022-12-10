#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

BEGIN(Client)

class CM_Pigs final : public CM_Monster
{
private:
	CM_Pigs(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Pigs(const CM_Pigs& rhs);
	virtual ~CM_Pigs() = default;

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
	void				Idle_Tick();
	void				Move_Tick();
	void				Attack_Tick(const _double& TimeDelta);
	void				Hit_Tick();
	void				Die_Tick();

private:
	_double				m_dBullet_TimeAcc = 0;		// 총알 을 몇 초마다 발사할 것 인지.

public:
	static	CM_Pigs*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free()						override;
};

END