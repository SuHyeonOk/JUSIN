#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "E_DieCenter.h"
#include "E_Burst.h"

BEGIN(Client)

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)

public:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	void	Effect_Paint_Create(CE_Burst::EFFECTINFO::TEXTURETYPE eTextureType, _float3 f3Pos, const _float3 & f3Color);
	void	Effect_Swim_Create(_float3 f3Pos);
	void	Effect_Hit_Create(_float3 f3Pos);
	void	Beneficial(_float3 f3Pos, _float3 f3Color);
	void	Food_Hp(_float3 f3Pos);
	void	Food_Up(_float3 f3Pos);
	void	Change_Ink(_float3 f3Pos);
	void	Change_Smoke(_float3 f3Pos, _float3 f3Color);
	void	Skill_Marceline_Waves_Create(_float3 f3Pos);
	void	Skill_Marceline_Sound_Create(_float3 f3Pos);
	void	DieCenter_Create(CE_DieCenter::DIECENTERINFO tDieCenter);

public:
	virtual void Free() override;
};

END