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

	_float	fRandomNumberX = CUtilities_Manager::GetInstance()->Get_Random(-0.5f, 0.5f);
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

	CE_Skill_Marceline_Sound::EFFECTINFO eEffectInfo;
	eEffectInfo.eEffectType = CE_Skill_Marceline_Sound::EFFECTINFO::HP;
	eEffectInfo.f3Pos = f3Size;
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

	_float	fRandomNumber = CUtilities_Manager::GetInstance()->Get_Random(-0.0f, 360.f);

	_vector vRandomNomal = XMVector3Normalize(XMVectorSet(CUtilities_Manager::GetInstance()->Get_Random(-360.f, 360.f),
		CUtilities_Manager::GetInstance()->Get_Random(-360.f, 360.f),
		CUtilities_Manager::GetInstance()->Get_Random(-360.f, 360.f), 1.0f));

	_float4 f4RandomNomal;
	XMStoreFloat4(&f4RandomNomal, vRandomNomal);

	_vector vInPos = XMLoadFloat4(&_float4(f3Size.x, f3Size.y, f3Size.z, 1.0f));

	_vector vRandomPos = XMVector3Dot(vRandomNomal, vInPos);

	_float4 f4RandomPos;
	XMStoreFloat4(&f4RandomPos, vRandomPos);

	//const POINT ptPos, const float nRadius, const float ptSize;
	//{
	//	POINT ptPoint;

	//	//���� �Լ�
	//	std::random_device rn;			//�õ� ��
	//	std::mt19937_64 rnd(rn());	//random_device�� �õ� ������ �Ͽ� �ʱ�ȭ

	//								//������ ������ 0~360 ���� ������ ����
	//	std::uniform_real_distribution<float> nDist(0.0f, 360.0f);
	//	float fTempDegree = nDist(rnd);

	//	//�������� ������ 0���� �Է¹��� �� ���� ������ ����
	//	std::uniform_real_distribution<float> fDist(0.0f, nRadius);
	//	float fTempRadius = fDist(rnd);

	//	//�������� ���� ���� ���������� �� ���� ��ġ ���
	//	ptPoint.x = _SCAST(long, cos(fTempDegree * DEG2RAD) * fTempRadius);
	//	ptPoint.y = _SCAST(long, sin(fTempDegree * DEG2RAD) * fTempRadius);

	//	//�Է� ���� �� ��������
	//	ptPoint.x += ptPos.x;
	//	ptPoint.y += ptPos.y;

	//	this->DrawPoint2D(ptPoint, ptSize);
	//}

	//f3Size = _float3(f3Size.x + fRandomNumber, f3Size.y, f3Size.z);

	//if (0 == fRandomPos)
	//	f3Size = _float3(f3Size.x + fRandomNumber, f3Size.y, f3Size.z);
	//else if (1 == fRandomPos)
	//	f3Size = _float3(f3Size.x - fRandomNumber, f3Size.y, f3Size.z);
	//else if (2 == fRandomPos)
	//	f3Size = _float3(f3Size.x, f3Size.y + fRandomNumber, f3Size.z);
	//else if (3 == fRandomPos)
	//	f3Size = _float3(f3Size.x, f3Size.y - fRandomNumber, f3Size.z);
	//else if (4 == fRandomPos)
	//	f3Size = _float3(f3Size.x + fRandomNumber, f3Size.y + fRandomNumber, f3Size.z);
	//else if (5 == fRandomPos)
	//	f3Size = _float3(f3Size.x - fRandomNumber, f3Size.y - fRandomNumber, f3Size.z);
	//else if (6 == fRandomPos)
	//	f3Size = _float3(f3Size.x + fRandomNumber, f3Size.y - fRandomNumber, f3Size.z);
	//else if (7 == fRandomPos)
	//	f3Size = _float3(f3Size.x - fRandomNumber, f3Size.y + fRandomNumber, f3Size.z);

	CE_Skill_Marceline_Sound::EFFECTINFO eEffectInfo;
	eEffectInfo.eEffectType = CE_Skill_Marceline_Sound::EFFECTINFO::INK;
	eEffectInfo.f3Pos = _float3(f4RandomPos.x, f4RandomPos.y , f4RandomPos.z);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Texture_Effect"), TEXT("Prototype_GameObject_E_Skill_Marceline_Sound"), &eEffectInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Manager::Change_Smoke(_float3 f3Size)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_Smoke::SMOKEINFO	tSmokeInfo;
	tSmokeInfo.f3Pos = f3Size;

	_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// ��������
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(fRandomAxis));
	_vector vLook = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	vLook = XMVector3TransformCoord(vLook, RotationMatrix);		// Look �� ���� �ѱ��.
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

	CE_Skill_Marceline_Sound::EFFECTINFO eEffectInfo;
	eEffectInfo.eEffectType = CE_Skill_Marceline_Sound::EFFECTINFO::SOUND;
	eEffectInfo.f3Pos = f3Size;
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
