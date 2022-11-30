#include "stdafx.h"
#include "..\public\Level_GamePlay.h"

#include <fstream>
#include "Imgui_PropertyEditor.h"

#include "GameInstance.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Garden(TEXT("Layer_Garden"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	ImGuiTest();
	
}

void CLevel_GamePlay::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.isEnable = true;
	///*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	//LightDesc.vPosition = _float4(5.f, 3.f, 5.f, 1.f);
	//LightDesc.fRange = 10.0f;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"), &_float3(-50.f, 0.f, -20.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map_Garden(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Map_Garden"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel_GamePlay::ImGuiTest()
{
	ImGui::Begin("GamePlayTool");

#pragma region Food
	const _char* FoodName[] = { "Royal_Tart", "Burrito" };
	static int iFoodNum = 0;
	ImGui::Combo("##2", &iFoodNum, FoodName, IM_ARRAYSIZE(FoodName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();
	
	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = {f4MousePos.x, f4MousePos.y, f4MousePos.z};

		if (0 == iFoodNum)
		{
			stFoodName = L"Royal_Tart__";
			stFoodName += to_wstring(m_iRoyal_TartCount);

			m_szFoodName = stFoodName.c_str();
		
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szFoodName, TEXT("Prototype_GameObject_Royal_Tart"), &_float3(m_f3ClickPos))))
				return;

			m_iRoyal_TartCount++;
		}

		if (1 == iFoodNum)
		{
			stFoodName = L"Burrito__";
			stFoodName += to_wstring(m_iRoyal_TartCount);

			m_szFoodName = stFoodName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szFoodName, TEXT("Prototype_GameObject_Burrito"), &_float3(m_f3ClickPos))))
				return;		

			m_iBurrito++;
		}
	}

	if (ImGui::Button("Save"))
	{
		ofstream ofs("../../Data/Food.txt", ios::out | ios::app);
		if (ofs.fail())
		{
			MSG_BOX("Failed to load File");
			return;
		}

		std::string str = "";
		str.assign(stFoodName.begin(), stFoodName.end());

		ofs << str << " | " << m_f3ClickPos.x << " | " << m_f3ClickPos.y << " | " << m_f3ClickPos.z << "\n";
	}

	if (ImGui::Button("Load"))
	{

	}

#pragma endregion Food




	if (pGameInstance->Key_Down(DIK_R))
	{
		cout << FoodName << " | " << iFoodNum << endl;
	}

	RELEASE_INSTANCE(CGameInstance);

	ImGui::End();

	return;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}

