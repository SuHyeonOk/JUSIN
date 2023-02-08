#pragma once

#include "Client_Defines.h"
#include "N_NPC.h"

BEGIN(Client)

class CN_BMO final : public CN_NPC
{
private:
	CN_BMO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CN_BMO(const CN_BMO& rhs);
	virtual ~CN_BMO() = default;

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
	void		Talk_Tick();
	
public:
	_int		m_iTalk_Count = 0;

public:
	static	CN_BMO*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free()						override;
};

END