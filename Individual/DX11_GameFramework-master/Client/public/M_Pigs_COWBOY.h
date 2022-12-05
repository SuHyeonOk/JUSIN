#pragma once

#include "Client_Defines.h"
#include "M_Monster.h"

BEGIN(Client)

class CM_Pigs_COWBOY final : public CM_Monster
{
private:
	CM_Pigs_COWBOY(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Pigs_COWBOY(const CM_Pigs_COWBOY& rhs);
	virtual ~CM_Pigs_COWBOY() = default;

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
	void				Player_Follow(const _double& TimeDelta);

public:
	static	CM_Pigs_COWBOY*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free()						override;
};

END