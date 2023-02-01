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
	HRESULT		Effect_Potal_StarColor_Create(const _float3 & f3Pos, const _float3 & f3Color = _float3(1.0f, 1.0f, 1.0f));
	HRESULT		Effect_Potal_Star_Create(const _float3 & f3Pos);

	HRESULT		Effect_Boss_CutScene(const _float3 & f3Pos);
	HRESULT		Effect_Boss_SkeletonBig(const _float3 & f3Pos);
	HRESULT		Effect_Boss_Potals_Create(const _float3 & f3Pos);
	HRESULT		Effect_Boss_Potal_Create(const _float3 & f3Pos);
	HRESULT		Effect_Color_Boss_Smoke_Create(const _float3 & f3Pos, const _float3 & f3Color = _float3(1.0f, 1.0f, 1.0f));
	HRESULT		Effect_Color_Skeleeton_Create(const _float3 & f3Pos);
	HRESULT		Effect_Burn_Fire_Create(const _float3 & f3Pos);
	HRESULT		Effect_Wave_Fire_Create(const _float3 & f3Pos);
	HRESULT		Effect_Small_Fire_Create(const _float3 & f3Pos);
	HRESULT		Effect_Color_Hit_Create(const _float3 & f3Pos, const _float3 & f3Color = _float3(1.0f, 1.0f, 1.0f));
	HRESULT		Effect_Boom_Fire_Create(const _float3 & f3Pos);

	HRESULT		Effect_JakeSon_Transform_Create(const _float3 & f3Pos);
	HRESULT		Effect_JakeSon_Create(const _float3 & f3Pos);
	HRESULT		Effect_Coin_Create(_float3 f3Pos);
	HRESULT		Effect_Shield_Create(_float3 f3Pos);

	HRESULT		Effect_Star_Create(_float3 f3Pos, _int iCount = 7);
	HRESULT		Effect_Star3_Count(_float3 f3Pos, const _float3 & f3Color = _float3(1.0f, 1.0f, 1.0f), _int iCount = 20);
	HRESULT		Effect_StarRandom_Create(_float3 f3Pos, const _float3 & f3Color);

	HRESULT		Effect_Paint_Circle_Create(_float3 f3Pos, const _float3 & f3Color);
	HRESULT		Effect_Paint_Firecracker_Create(_float3 f3Pos, const _float3 & f3Color);
	HRESULT		Effect_Swim_Create(_float3 f3Pos);
	HRESULT		Effect_Hit_Create(_float3 f3Pos);
	HRESULT		Beneficial(_float3 f3Pos, _float3 f3Color);
	HRESULT		Food_MinusHp(_float3 f3Pos);
	HRESULT		Food_Hp(_float3 f3Pos);
	HRESULT		Food_Up(_float3 f3Pos);
	HRESULT		Effect_Ink(_float3 f3Pos, _float3 f3Color);
	HRESULT		Effect_Random_Ink(_float3 f3Pos, _float3 f3Color);
	HRESULT		Effect_Smoke_Count(_float3 f3Pos, _float3 f3Color, _int iCount = 10, _float2 f2Size = {0.3f, 1.0f});
	HRESULT		Skill_Marceline_Waves_Create(_float3 f3Pos);
	HRESULT		Skill_Marceline_Sound_Create(_float3 f3Pos);
	HRESULT		DieCenter_Create(CE_DieCenter::DIECENTERINFO tDieCenter);

public:
	virtual void Free() override;
};

END