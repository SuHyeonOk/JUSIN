#include "stdafx.h"
#include "..\public\Level_GamePlay.h"

#include "GameInstance.h"

#include <fstream>
#include "Imgui_PropertyEditor.h"
#include "DataManager.h"

#include "Food.h"
#include "Coin.h"

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

	FoodLoad();

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

	const _char* ItmeName[] = { "Empty", "Food", "Coin" };
	static int iItemNum = 0;
	ImGui::Combo("##2_ITEM", &iItemNum, ItmeName, IM_ARRAYSIZE(ItmeName));

	if(1 == iItemNum)
		ImGuiFood();
	else if(2 == iItemNum)
		ImGuiCoin();

	ImGui::End();

	return;
}

void CLevel_GamePlay::ImGuiFood()
{
#pragma region Food
	const _char* FoodName[] = { "Royal_Tart", "Burrito" };
	static int iFoodNum = 0;
	ImGui::Combo("##2_FOOD", &iFoodNum, FoodName, IM_ARRAYSIZE(FoodName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CFood::FOODINFO		fFoodInfo;
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();


	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z }; // 클릭시의 좌표 저장

		if (0 == iFoodNum)
		{
			fFoodInfo.eFoodKind = fFoodInfo.ROYAL_TART;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstItemName = L"Royal_Tart__";
			m_wstItemName += to_wstring(m_iRoyal_Tart_Count); // 문자열에 상수 더하기

			m_szItemName = m_wstItemName.c_str();	// wstring -> conat wchar*

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szItemName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iRoyal_Tart_Count++;
		}

		if (1 == iFoodNum)
		{
			fFoodInfo.eFoodKind = fFoodInfo.BURRITO;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstItemName = L"Burrito__";
			m_wstItemName += to_wstring(m_iBurrito_Count);

			m_szItemName = m_wstItemName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szItemName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iBurrito_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Food Save"))
	{
		wofstream fout("../../Data/Food.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		// fouttream 은 string 전용 / wfouttream 은 wstring 전용
		//std::string str = "";
		//str.assign(m_wstItemName.begin(), m_wstItemName.end());

		fout << m_wstItemName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();

		WinExec("notepad.exe ../../Data/Food.txt", SW_SHOW);
	}
#pragma endregion Food
}

void CLevel_GamePlay::ImGuiCoin()
{
	const _char* szCoinName[] = { "CoinBronze", "CoinSilver", "CoinGold" };
	static int iCoinNum = 0;
	ImGui::Combo("##2_COIN", &iCoinNum, szCoinName, IM_ARRAYSIZE(szCoinName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCoin::COININFO		tCoinInfo;
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();


	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z }; 

		if (0 == iCoinNum)
		{
			tCoinInfo.eCoinKind = tCoinInfo.COIN_BRONZE;
			tCoinInfo.fPos = m_f3ClickPos;

			m_wstItemName = L"CoinBronze__";
			m_wstItemName += to_wstring(m_iCoinBronzeCount);

			m_szItemName = m_wstItemName.c_str();	

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szItemName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinBronzeCount++;
		}
		else if (1 == iCoinNum)
		{
			tCoinInfo.eCoinKind = tCoinInfo.COIN_SILVER;
			tCoinInfo.fPos = m_f3ClickPos;

			m_wstItemName = L"CoinSilver__";
			m_wstItemName += to_wstring(m_iCoinSilverCount);

			m_szItemName = m_wstItemName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szItemName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinSilverCount++;
		}
		else if (2 == iCoinNum)
		{
			tCoinInfo.eCoinKind = tCoinInfo.COIN_GOLD;
			tCoinInfo.fPos = m_f3ClickPos;

			m_wstItemName = L"CoinGold__";
			m_wstItemName += to_wstring(m_iCoinGoldCount);

			m_szItemName = m_wstItemName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szItemName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinGoldCount++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Coin Save"))
	{
		wofstream fout("../../Data/Coin.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstItemName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();

		WinExec("notepad.exe ../../Data/Coin.txt", SW_SHOW);
	}
}

void CLevel_GamePlay::FoodLoad()
{
	wifstream		fin("../../Data/Food.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return;
	}

	_tchar szObjName[MAX_PATH] = L"";
	_tchar szObjPosX[MAX_PATH] = L"";
	_tchar szObjPosY[MAX_PATH] = L"";
	_tchar szObjPosZ[MAX_PATH] = L"";

	_float	fObjPosX = 0.f;
	_float	fObjPosY = 0.f;
	_float	fObjPosZ = 0.f;

	while (true)
	{
		fin.getline(szObjName, MAX_PATH, '|');
		fin.getline(szObjPosX, MAX_PATH, '|');
		fin.getline(szObjPosY, MAX_PATH, '|');
		fin.getline(szObjPosZ, MAX_PATH);

		if (fin.eof())
			break;

		fObjPosX = (_float)_tstof(szObjPosX);
		fObjPosY = (_float)_tstof(szObjPosY);
		fObjPosZ = (_float)_tstof(szObjPosZ);

		CDataManager::GetInstance()->Set_FoodInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}
	


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CFood::FOODINFO					tFoodInfo;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_FoodInfo();	
	_int iFoodVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tFoodInfo.eFoodKind = tFoodInfo.ROYAL_TART;
			tFoodInfo.fPos = pObjInfo.ObjPos;

			m_wstItemName = L"Royal_Tart__";
			m_wstItemName += to_wstring(i);

			wstring wstFoodNameTemp(pObjInfo.ObjName); // tchar -> wstring

			if (m_wstItemName == wstFoodNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return;
			}
		}

		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tFoodInfo.eFoodKind = tFoodInfo.BURRITO;
			tFoodInfo.fPos = pObjInfo.ObjPos;

			m_wstItemName = L"Burrito__";
			m_wstItemName += to_wstring(i);

			wstring wstFoodNameTemp(pObjInfo.ObjName);

			if (m_wstItemName == wstFoodNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return;
			}
		}		
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_GamePlay::CoinLoad()
{
	wifstream		fin("../../Data/Coin.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return;
	}

	_tchar szObjName[MAX_PATH] = L"";
	_tchar szObjPosX[MAX_PATH] = L"";
	_tchar szObjPosY[MAX_PATH] = L"";
	_tchar szObjPosZ[MAX_PATH] = L"";

	_float	fObjPosX = 0.f;
	_float	fObjPosY = 0.f;
	_float	fObjPosZ = 0.f;

	while (true)
	{
		fin.getline(szObjName, MAX_PATH, '|');
		fin.getline(szObjPosX, MAX_PATH, '|');
		fin.getline(szObjPosY, MAX_PATH, '|');
		fin.getline(szObjPosZ, MAX_PATH);

		if (fin.eof())
			break;

		fObjPosX = (_float)_tstof(szObjPosX);
		fObjPosY = (_float)_tstof(szObjPosY);
		fObjPosZ = (_float)_tstof(szObjPosZ);

		CDataManager::GetInstance()->Set_CoinInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCoin::COININFO					tCoinInfo;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_CoinInfo();
	_int iFoodVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tCoinInfo.eCoinKind = tCoinInfo.COIN_BRONZE;
			tCoinInfo.fPos = pObjInfo.ObjPos;

			m_wstItemName = L"CoinBronze__";
			m_wstItemName += to_wstring(i);

			wstring wstItemNameTemp(pObjInfo.ObjName);

			if (m_wstItemName == wstItemNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return;
			}
		}

		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tCoinInfo.eCoinKind = tCoinInfo.COIN_SILVER;
			tCoinInfo.fPos = pObjInfo.ObjPos;

			m_wstItemName = L"CoinSilver__";
			m_wstItemName += to_wstring(i);

			wstring wstItemNameTemp(pObjInfo.ObjName);

			if (m_wstItemName == wstItemNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return;
			}
		}

		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tCoinInfo.eCoinKind = tCoinInfo.COIN_GOLD;
			tCoinInfo.fPos = pObjInfo.ObjPos;

			m_wstItemName = L"CoinGold__";
			m_wstItemName += to_wstring(i);

			wstring wstItemNameTemp(pObjInfo.ObjName);

			if (m_wstItemName == wstItemNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
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

