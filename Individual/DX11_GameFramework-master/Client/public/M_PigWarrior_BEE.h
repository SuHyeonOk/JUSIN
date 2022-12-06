#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

BEGIN(Client)

class CM_PigWarrior_BEE final : public CM_Monster
{
private:
	CM_PigWarrior_BEE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_PigWarrior_BEE(const CM_PigWarrior_BEE& rhs);
	virtual ~CM_PigWarrior_BEE() = default;

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
	virtual void		Monster_Die()							override;

private:
	void				ToThe_Player(const _double& TimeDelta);

public:
	static	CM_PigWarrior_BEE*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free()						override;
};

END