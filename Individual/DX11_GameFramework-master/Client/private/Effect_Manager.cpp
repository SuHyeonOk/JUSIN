#include "stdafx.h"
#include "..\public\Effect_Manager.h"

#include "GameInstance.h"
#include "Utilities_Manager.h"

#include "E_Burst.h"
#include "E_Look_Grow.h"
#include "E_NoLook_Grow.h"
#include "E_Beneficial.h"
#include "E_Alpha_Change.h"
#include "E_Alpha_Rotation.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{
}

HRESULT CEffect_Manager::Effect_JakeSon_Create(const _float3 & f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Alpha_Change::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::JAKESON_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Alpha_Change"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Coin_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float	fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomY = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);

	_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, fRandomY, fRandomZ, 1.0f));

	_float fRandomRange = 0.0f;
	_vector vRandomPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	_float4 f4RandomPos;

	CE_Burst::EFFECTINFO	tEffetInfo;
	_float fRandomAxis = 0.0f;
	_matrix	RotationMatrix = XMMatrixIdentity();
	_vector vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// 잎

	fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f);
	vRandomPos = vTempPos * fRandomRange;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	tEffetInfo.f3Pos = _float3(f3Pos.x + f4RandomPos.x, f3Pos.y + 1.0f + f4RandomPos.y, f3Pos.z - 1.0f);
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::LEAF_TEXTURE;

	fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tEffetInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
		return E_FAIL;

	// 하트

	fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f);
	vRandomPos = vTempPos * fRandomRange;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	tEffetInfo.f3Pos = _float3(f3Pos.x + f4RandomPos.x, f3Pos.y + 1.0f + f4RandomPos.y, f3Pos.z - 1.1f);
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::HEART_TEXTURE;
	fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tEffetInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
		return E_FAIL;

	// 꽃

	fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f);
	vRandomPos = vTempPos * fRandomRange;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	tEffetInfo.f3Pos = _float3(f3Pos.x + f4RandomPos.x, f3Pos.y + 1.0f + f4RandomPos.y, f3Pos.z - 1.2f);
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::FLOWER_TEXTURE;
	fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tEffetInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Shield_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	CE_Alpha_Rotation::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Alpha_Rotation::EFFECTINFO::SHIELD_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Alpha_Rotation"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Star_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Burst::EFFECTINFO	tEffetInfo;
	tEffetInfo.f3Pos = f3Pos;
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::STAR_TEXTURE;

	_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tEffetInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Star3_Create(_float3 f3Pos, const _float3 & f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Burst::EFFECTINFO	tEffetInfo;
	tEffetInfo.f3Pos = f3Pos;
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE;
	tEffetInfo.f3Color = f3Color;

	_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tEffetInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Star3Count_Create(_float3 f3Pos, const _float3 & f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Burst::EFFECTINFO	tEffetInfo;
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::STAR3_TEXTURE;
	tEffetInfo.f3Pos = f3Pos;
	tEffetInfo.f3Color = f3Color;

	for (_int i = 0; i < 10; ++i)
	{
		_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
		_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
		_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
		XMStoreFloat4(&tEffetInfo.f4Look, vLook);

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_StarRandom_Create(_float3 f3Pos, const _float3 & f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::STAR3_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	tEffectInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Paint_Circle_Create(_float3 f3Pos, const _float3 & f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::PAINT_CIRCLE_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	tEffectInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Paint_Firecracker_Create(_float3 f3Pos, const _float3 & f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::PAINT_FIRECRAKER_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	tEffectInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;

	return S_OK;
}

HRESULT CEffect_Manager::Effect_Swim_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_NoLook_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_NoLook_Grow::EFFECTINFO::SWIM_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_NoLook_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Hit_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Alpha_Change::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Alpha_Change::EFFECTINFO::TEXTURETYPE::HIT_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Alpha_Change"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Beneficial(_float3 f3Pos, _float3 f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Beneficial::BENEFICIALINFO	tBeneficlalInfo;
	tBeneficlalInfo.f3Pos = f3Pos;
	tBeneficlalInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Beneficial"), &tBeneficlalInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Food_Hp(_float3 f3Pos)
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

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::HP_TEXTURE;
	tEffectInfo.f3Pos = _float3(f3Pos.x + f4RandomPos.x, f3Pos.y + f4RandomPos.y, f3Pos.z + f4RandomPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Food_Up(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Food_Up"), &f3Pos)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Ink(_float3 f3Pos, _float3 f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::INK_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	tEffectInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Random_Ink(_float3 f3Pos, _float3 f3Color)
{
	// 별로.. '-'
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomY = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f);
	_float	fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);

	_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, fRandomY, fRandomZ, 1.0f));

	_float fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 2.0f);		// 거리

	_vector vRandomPos = vTempPos * fRandomRange;
	_float4 f4RandomPos;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::INK_RANDOM_TEXTURE;
	tEffectInfo.f3Pos = _float3(f3Pos.x + f4RandomPos.x, f3Pos.y + f4RandomPos.y, f3Pos.z + f4RandomPos.z);
	tEffectInfo.f3Color = f3Color;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Effect_Smoke(_float3 f3Pos, _float3 f3Color)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Burst::EFFECTINFO	tEffetInfo;
	tEffetInfo.f3Pos = f3Pos;
	tEffetInfo.f3Color = f3Color;
	tEffetInfo.eTextureType = CE_Burst::EFFECTINFO::TEXTURETYPE::SMOKE_TEXUTRE;

	_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look 을 만들어서 넘긴다.
	XMStoreFloat4(&tEffetInfo.f4Look, vLook);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Burst"), &tEffetInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Skill_Marceline_Waves_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_NoLook_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_NoLook_Grow::EFFECTINFO::MARVELINE_TEXTURE;
	tEffectInfo.f3Pos = f3Pos;
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_NoLook_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::Skill_Marceline_Sound_Create(_float3 f3Pos)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomX = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);
	_float	fRandomY = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.0f);
	_float	fRandomZ = CUtilities_Manager::GetInstance()->Get_Random(-1.0f, 1.0f);

	_vector vTempPos = XMVector3Normalize(XMVectorSet(fRandomX, fRandomY, fRandomZ, 1.0f));

	_float fRandomRange = CUtilities_Manager::GetInstance()->Get_Random(0.0f, 1.5f);

	_vector vRandomPos = vTempPos * fRandomRange;
	_float4 f4RandomPos;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	CE_Look_Grow::EFFECTINFO tEffectInfo;
	tEffectInfo.eTextureType = CE_Look_Grow::EFFECTINFO::SOUND_TEXTURE;
	tEffectInfo.f3Pos = _float3(f3Pos.x + f4RandomPos.x, f3Pos.y + f4RandomPos.y, f3Pos.z + f4RandomPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Look_Grow"), &tEffectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEffect_Manager::DieCenter_Create(CE_DieCenter::DIECENTERINFO tDieCenter)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float	fRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(-0.1f, 0.1f);

	CE_DieCenter::DIECENTERINFO	tDieCenterInfo;
	tDieCenterInfo.eMonsterKind = tDieCenter.eMonsterKind;
	tDieCenterInfo.f3Pos = _float3(tDieCenter.f3Pos.x + fRandomNumber, tDieCenter.f3Pos.y + fRandomNumber, tDieCenter.f3Pos.z);
 	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_DieCenter"), &tDieCenterInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEffect_Manager::Free()
{
}
