#include "stdafx.h"
#include "..\public\Level_Skeleton_Boss.h"

#include <fstream> // @
#include "Imgui_PropertyEditor.h"	// @

#include "GameInstance.h"
#include "DataManager.h"
#include "Camera_Dynamic.h"
#include "Obj_Manager.h"

#include "O_TextureObject.h"
#include "M_Monster.h"
#include "N_NPC.h"
#include "Food.h"
#include "Coin.h"
#include "Page.h"
#include "O_Collider.h"
#include "E_Alpha_Change.h"

CLevel_Skleton_Boss::CLevel_Skleton_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Skleton_Boss::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
#ifdef F2_SKELETON

	CObj_Manager::GetInstance()->Set_NextLevel(false);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Finn(TEXT("Layer_Finn"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Jake(TEXT("Layer_Jake"))))
		return E_FAIL;
#else
	if (FAILED(Ready_PreviousData()))
		return E_FAIL;
#endif

	if (FAILED(Ready_Layer_Boss(TEXT("Layer_Gary_Boss"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox_Skeleton_Boss"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Skeleton_Boss"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Skleton_Boss::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_9))
		pGameInstance->Set_ChangeLight(true);	// 변경한 데이터 사용
	if (pGameInstance->Key_Down(DIK_0))
		pGameInstance->Set_ChangeLight(false);	// 일반 사용


	static _float					arrfPosition[4] = { 0.0f };
	//static _float					asdfd[4] = { 0.0f };
	static _float					Range = { 0.0f };
	static _float					arrfDiffuse[4] = { 0.0f };
	static _float					arrfAmbient[4] = { 0.0f };

	ImGui::InputFloat4("Positiont", arrfPosition);
	//ImGui::InputFloat4("asdfd", asdfd);
	ImGui::InputFloat("Range", &Range);
	ImGui::InputFloat4("Diffuse", arrfDiffuse);
	ImGui::InputFloat4("Ambient", arrfAmbient);

	if (ImGui::Button("Set_Light"))
	{
		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof LightDesc);

		//LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
		//LightDesc.isEnable = true;
		//LightDesc.vDirection = _float4(arrfPosition[0], arrfPosition[1], arrfPosition[2], arrfPosition[3]);
		//LightDesc.vDiffuse = _float4(asdfd[0], asdfd[1], asdfd[2], asdfd[3]);
		//LightDesc.vAmbient = _float4(arrfDiffuse[0], arrfDiffuse[1], arrfDiffuse[2], arrfDiffuse[3]);
		//LightDesc.vSpecular = _float4(arrfAmbient[0], arrfAmbient[1], arrfAmbient[2], arrfAmbient[3]);

		LightDesc.eType = LIGHTDESC::TYPE_POINT;
		LightDesc.isEnable = true;
		LightDesc.vPosition = _float4(arrfPosition[0], arrfPosition[1], arrfPosition[2], arrfPosition[3]);
		LightDesc.fRange = Range;
		LightDesc.vDiffuse = _float4(arrfDiffuse[0], arrfDiffuse[1], arrfDiffuse[2], arrfDiffuse[3]);
		LightDesc.vAmbient = _float4(arrfAmbient[0], arrfAmbient[1], arrfAmbient[2], arrfAmbient[3]);
		LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(pGameInstance->Set_ChangeLight(m_pDevice, m_pContext, LightDesc, 3)))
			return;
	}

	if (ImGui::Button("Add_Light"))
	{
		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHTDESC::TYPE_POINT;
		LightDesc.isEnable = true;
		LightDesc.vPosition = _float4(arrfPosition[0], arrfPosition[1], arrfPosition[2], arrfPosition[3]);
		LightDesc.fRange = Range;
		LightDesc.vDiffuse = _float4(arrfDiffuse[0], arrfDiffuse[1], arrfDiffuse[2], arrfDiffuse[3]);
		LightDesc.vAmbient = _float4(arrfAmbient[0], arrfAmbient[1], arrfAmbient[2], arrfAmbient[3]);
		LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(pGameInstance->Add_ChangeLight(m_pDevice, m_pContext, LightDesc, 0)))
			return;
	}

	if (ImGui::Button("Light Save"))
	{
		wofstream fout("../../Data/SkeletonBoss_Light.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << arrfPosition[0] << L"|" << arrfPosition[1] << L"|" << arrfPosition[2] << L"|" <<
			Range << L"|" <<
			arrfDiffuse[0] << L"|" << arrfDiffuse[1] << L"|" << arrfDiffuse[2] << "\n";

		fout.close();
	}

	if (ImGui::Button("Light txt"))
		WinExec("notepad.exe ../../Data/SkeletonBoss_Light.txt", SW_SHOW);



	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_Skleton_Boss::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

}

HRESULT CLevel_Skleton_Boss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Reay_ChangeLight();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_PreviousData()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CNavigation * pObjNavigationCom = nullptr;

	CTransform * pObjTransformCom;
	pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(7.0f, 0.0f, 0.0f));

	pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(8.0f, 0.0f, 0.0f));

	pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(7.0f, 0.0f, 0.0f));

	pObjNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));	
	pObjNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton_Boss.txt"));
	pObjNavigationCom->Set_CellIndex(22);

	pObjNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));
	pObjNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton_Boss.txt"));
	pObjNavigationCom->Set_CellIndex(22);

	pObjNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_BackGround"), TEXT("Com_Navigation"), 0));
	pObjNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton_Boss.txt"));

	CObj_Manager::GetInstance()->Set_NextLevel(false);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CO_Collider::COLLIDERINFO		tColliderInfo;
	tColliderInfo.eType = CO_Collider::COLLIDERINFO::CUTSCENE_TWO;
	tColliderInfo.f3Pos = _float3(7.0f, 0.f, 0.f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Collider"), TEXT("Prototype_GameObject_O_Collider"), &tColliderInfo)))
		return E_FAIL;

	tColliderInfo.eType = CO_Collider::COLLIDERINFO::BOSS;
	tColliderInfo.f3Pos = _float3(2.7f, 0.0f, 13.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Collider"), TEXT("Prototype_GameObject_O_Collider"), &tColliderInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Terrain"), &_float3(-50.f, 0.f, -20.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Dynamic::CAMERAINFO eCameraInfo;
	eCameraInfo.eLevel = LEVEL_SKELETON_BOSS;
	eCameraInfo.f3Pos = _float3(6.0f, 3.0f, 12.6f);
	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &eCameraInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_Finn(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Finn"), &_float3(7.0f, 0.f, 0.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_Jake(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Jake"), &_float3(8.0f, 0.f, 0.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK; 
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_Boss(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, pLayerTag, TEXT("Prototype_GameObject_M_Gary_Boss"), &_float3(6.2f, 1.5f, 20.0f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON_BOSS, pLayerTag, TEXT("Prototype_GameObject_Map_Skeleton_Boss"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton_Boss::Reay_ChangeLight()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 1.f);

	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(pGameInstance->Add_ChangeLight(m_pDevice, m_pContext, LightDesc, i)))
			return E_FAIL;
	}

	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;				// 포인트 조명
	//LightDesc.isEnable = true;
	//LightDesc.vPosition = _float4(6.f, 0.f, 20.f, 1.f);	// 위치
	//LightDesc.fRange = 10.0f;								// 조명 거리
	//LightDesc.vDiffuse = _float4(0.6f, 0.4f, 0.4f, 1.f);		// 색상
	//LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 0.2f);	// 세기
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(pGameInstance->Add_ChangeLight(m_pDevice, m_pContext, LightDesc, 0)))
	//	return E_FAIL;

	wifstream		fin("../../Data/SkeletonBoss_Light.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
	}

	_tchar szIndx[MAX_PATH] = L"";
	_tchar szObjPosX[MAX_PATH] = L"";
	_tchar szObjPosY[MAX_PATH] = L"";
	_tchar szObjPosZ[MAX_PATH] = L"";
	_tchar szRange[MAX_PATH] = L"";
	_tchar szDiffuseX[MAX_PATH] = L"";
	_tchar szDiffuseY[MAX_PATH] = L"";
	_tchar szDiffuseZ[MAX_PATH] = L"";

	_int		iIndex;
	LIGHTDESC	tLightDesc;

	while (true)
	{
		fin.getline(szIndx, MAX_PATH, '|');
		fin.getline(szObjPosX, MAX_PATH, '|');
		fin.getline(szObjPosY, MAX_PATH, '|');
		fin.getline(szObjPosZ, MAX_PATH, '|');
		fin.getline(szRange, MAX_PATH, '|');
		fin.getline(szDiffuseX, MAX_PATH, '|');
		fin.getline(szDiffuseY, MAX_PATH, '|');
		fin.getline(szDiffuseZ, MAX_PATH);

		if (fin.eof())
			break;

		iIndex = (_int)_tstof(szIndx);
		tLightDesc.vPosition.x = (_float)_tstof(szObjPosX);
		tLightDesc.vPosition.y = (_float)_tstof(szObjPosY);
		tLightDesc.vPosition.z = (_float)_tstof(szObjPosZ);
		tLightDesc.fRange = (_float)_tstof(szRange);
		tLightDesc.vDiffuse.x = (_float)_tstof(szDiffuseX);
		tLightDesc.vDiffuse.y = (_float)_tstof(szDiffuseY);
		tLightDesc.vDiffuse.z = (_float)_tstof(szDiffuseZ);

		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHTDESC::TYPE_POINT;
		LightDesc.isEnable = true;
		LightDesc.vPosition = _float4(tLightDesc.vPosition.x, tLightDesc.vPosition.y, tLightDesc.vPosition.z, 1.0f);
		LightDesc.fRange = tLightDesc.fRange;
		LightDesc.vDiffuse = _float4(tLightDesc.vDiffuse.x, tLightDesc.vDiffuse.y, tLightDesc.vDiffuse.z, 0.0f);
		LightDesc.vAmbient = _float4(1.0f, 0.0f, 0.0f, 1.0f);
		LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(pGameInstance->Add_ChangeLight(m_pDevice, m_pContext, LightDesc, iIndex)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Skleton_Boss * CLevel_Skleton_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Skleton_Boss*		pInstance = new CLevel_Skleton_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Skleton_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Skleton_Boss::Free()
{
	__super::Free();


}

