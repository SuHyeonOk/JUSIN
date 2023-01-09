#include "stdafx.h"
#include "..\public\Effect_Manager.h"

#include "GameInstance.h"
#include "Utilities_Manager.h"

#include "E_Smoke.h"
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

void CEffect_Manager::Food_Up(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Food_Up"), &f3Size)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Change_Smoke(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Smoke::SMOKEINFO	tSmokeInfo;
	tSmokeInfo.f3Pos = f3Size;

	_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector4Transform(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
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

	_float	fRandomNumberX = CUtilities_Manager::GetInstance()->Get_Random(-2.0f, 2.0f);
	_float	fRandomNumberY = CUtilities_Manager::GetInstance()->Get_Random(0.6f, 2.0f);
	_float	fRandomNumberZ = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 2.0f);

	_int fRandomPos = CUtilities_Manager::GetInstance()->Get_Random(0, 4);

	if (0 == fRandomPos)
		f3Size = _float3(f3Size.x + fRandomNumberX, f3Size.y + fRandomNumberY, f3Size.z + fRandomNumberZ);
	else if (1 == fRandomPos)
		f3Size = _float3(f3Size.x - fRandomNumberX, f3Size.y + fRandomNumberY, f3Size.z + fRandomNumberZ);
	else if (2 == fRandomPos)
		f3Size = _float3(f3Size.x + fRandomNumberX, f3Size.y + fRandomNumberY, f3Size.z - fRandomNumberZ);
	else if (3 == fRandomPos)
		f3Size = _float3(f3Size.x - fRandomNumberX, f3Size.y + fRandomNumberY, f3Size.z - fRandomNumberZ);

	//_float	fRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);

	//f3Size.x *= fRandomNumber;
	//f3Size.y *= fRandomNumber;
	//f3Size.z *= fRandomNumber;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Skill_Marceline_Sound"), &f3Size)))
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
