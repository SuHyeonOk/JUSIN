#include "stdafx.h"
#include "..\public\Effect_Manager.h"

#include "GameInstance.h"
#include "Utilities_Manager.h"

#include "E_Smoke.h"
#include "E_Skill_Marceline_Sound.h"
#include "E_Beneficial.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{
}

void CEffect_Manager::Beneficial(_float3 f3Size, _float3 f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Beneficial::BENEFICIALINFO	tBeneficlalInfo;
	tBeneficlalInfo.f3Pos = f3Size;
	tBeneficlalInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Beneficial"), &tBeneficlalInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Food_Hp(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomY = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-0.2f, 0.2f);

	_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, fRandomY, fRandomZ, 1.0f));

	_float fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 0.7f);

	_vector vRandomPos = vTempPos * fRandomRange;
	_float4 f4RandomPos;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	CE_Skill_Marceline_Sound::EFFECTINFO eEffectInfo;
	eEffectInfo.eEffectType = CE_Skill_Marceline_Sound::EFFECTINFO::HP;
	eEffectInfo.f3Pos = _float3(f3Size.x + f4RandomPos.x, f3Size.y + f4RandomPos.y, f3Size.z + f4RandomPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Skill_Marceline_Sound"), &eEffectInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Food_Up(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Food_Up"), &f3Size)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Change_Ink(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomY = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);

	_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, fRandomY, fRandomZ, 1.0f));

	_float fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f);

	_vector vRandomPos = vTempPos * fRandomRange;
	_float4 f4RandomPos;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	CE_Skill_Marceline_Sound::EFFECTINFO eEffectInfo;
	eEffectInfo.eEffectType = CE_Skill_Marceline_Sound::EFFECTINFO::INK;
	eEffectInfo.f3Pos = _float3(f3Size.x + f4RandomPos.x, f3Size.y + f4RandomPos.y , f3Size.z + f4RandomPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Skill_Marceline_Sound"), &eEffectInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Change_Smoke(_float3 f3Size, _float3 f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Smoke::SMOKEINFO	tSmokeInfo;
	tSmokeInfo.f3Pos = f3Size;
	tSmokeInfo.f3Color = f3Color;

	_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tSmokeInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Smoke"), &tSmokeInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Skill_Marceline_Waves_Create(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Skill_Marceline_Waves"), &f3Size)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Skill_Marceline_Sound_Create(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomY = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-0.2f, 0.2f);

	_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, fRandomY, fRandomZ, 1.0f));

	_float fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 2.0f);

	_vector vRandomPos = vTempPos * fRandomRange;
	_float4 f4RandomPos;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	CE_Skill_Marceline_Sound::EFFECTINFO eEffectInfo;
	eEffectInfo.eEffectType = CE_Skill_Marceline_Sound::EFFECTINFO::SOUND;
	eEffectInfo.f3Pos = _float3(f3Size.x + f4RandomPos.x, f3Size.y + f4RandomPos.y, f3Size.z + f4RandomPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Skill_Marceline_Sound"), &eEffectInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::DieCenter_Create(CE_DieCenter::DIECENTERINFO tDieCenter)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(-0.1f, 0.1f);

	CE_DieCenter::DIECENTERINFO	tDieCenterInfo;
	tDieCenterInfo.eMonsterKind = tDieCenter.eMonsterKind;
	tDieCenterInfo.f3Pos = _float3(tDieCenter.f3Pos.x + fRandomNumber, tDieCenter.f3Pos.y + fRandomNumber, tDieCenter.f3Pos.z);
 	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_DieCenter"), &tDieCenterInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Free()
{
}
