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
	HRESULT		Effect_Star_Create(_float3 f3Pos);
	HRESULT		Effect_Star3_Create(_float3 f3Pos, const _float3 & f3Color);
	HRESULT		Effect_StarRandom_Create(_float3 f3Pos, const _float3 & f3Color);

	HRESULT		Effect_Paint_Circle_Create(_float3 f3Pos, const _float3 & f3Color);
	HRESULT		Effect_Paint_Firecracker_Create(_float3 f3Pos, const _float3 & f3Color);
	HRESULT		Effect_Swim_Create(_float3 f3Pos);
	HRESULT		Effect_Hit_Create(_float3 f3Pos);
	HRESULT		Beneficial(_float3 f3Pos, _float3 f3Color);
	HRESULT		Food_Hp(_float3 f3Pos);
	HRESULT		Food_Up(_float3 f3Pos);
	HRESULT		Effect_Ink(_float3 f3Pos, _float3 f3Color);
	HRESULT		Effect_Random_Ink(_float3 f3Pos, _float3 f3Color);
	HRESULT		Effect_Smoke(_float3 f3Pos, _float3 f3Color);
	HRESULT		Skill_Marceline_Waves_Create(_float3 f3Pos);
	HRESULT		Skill_Marceline_Sound_Create(_float3 f3Pos);
	HRESULT		DieCenter_Create(CE_DieCenter::DIECENTERINFO tDieCenter);

public:
	virtual void Free() override;
};

END