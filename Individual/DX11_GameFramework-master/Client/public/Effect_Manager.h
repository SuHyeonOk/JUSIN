#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "E_DieCenter.h"

BEGIN(Client)

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)

public:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	void	Effect_Hit_Create(_float3 f3Size);
	void	Beneficial(_float3 f3Size, _float3 f3Color);
	void	Food_Hp(_float3 f3Size);
	void	Food_Up(_float3 f3Size);
	void	Change_Ink(_float3 f3Size);
	void	Change_Smoke(_float3 f3Size, _float3 f3Color);
	void	Skill_Marceline_Waves_Create(_float3 f3Size);
	void	Skill_Marceline_Sound_Create(_float3 f3Size);
	void	DieCenter_Create(CE_DieCenter::DIECENTERINFO tDieCenter);

public:
	virtual void Free() override;
};

END