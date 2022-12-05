#include "stdafx.h"
#include "..\public\Level_GamePlay.h"

#include <fstream> // @
#include "Imgui_PropertyEditor.h"	// @

#include "GameInstance.h"
#include "DataManager.h"
#include "Camera_Dynamic.h"

#include "M_Monster.h"
#include "Food.h"
#include "Coin.h"
#include "Page.h"

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

	if (FAILED(Ready_Layer_Finn(TEXT("Layer_Finn"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Jake(TEXT("Layer_Jake"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Garden(TEXT("Layer_Garden"))))
		return E_FAIL;

	// 파일 읽기
	FoodLoad();
	CoinLoad();
	PageLoad();
	MonsterLoad();

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	ImGuiTest(); // @ ImGui 를 사용하지 않을 때 주석!
	
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

	CCamera_Dynamic::CAMERAINFO eCameraInfo;
	eCameraInfo.eLevel = LEVEL_GAMEPLAY;
	eCameraInfo.f3Pos = _float3(-10.f, 0.f, -10.f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &eCameraInfo)))
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

HRESULT CLevel_GamePlay::Ready_Layer_Finn(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Finn"), &_float3(-10.f, 0.f, -10.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Jake(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Jake"), &_float3(-5.f, 0.f, -10.f))))
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

	const _char* ItmeName[] = { "Empty", "Food", "Coin", "Page", "Monster" };
	static int iItemNum = 0;
	ImGui::Combo("##2", &iItemNum, ItmeName, IM_ARRAYSIZE(ItmeName));

	if (1 == iItemNum)
		ImGuiFood();
	else if (2 == iItemNum)
		ImGuiCoin();
	else if (3 == iItemNum)
		ImGuiPage();
	else if (4 == iItemNum)
		ImGuiMonster();

	ImGui::End();

	return;
}

void CLevel_GamePlay::ImGuiFood()
{
#pragma region Food
	const _char* szObjName[] = { "Royal_Tart", "Burrito" };
	static int iObjNum = 0;
	ImGui::Combo("##2_FOOD", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CFood::FOODINFO		fFoodInfo;
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();


	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z }; // 클릭시의 좌표 저장

		if (0 == iObjNum)
		{
			fFoodInfo.eFoodKind = fFoodInfo.ROYAL_TART;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstObjName = L"Royal_Tart__";
			m_wstObjName += to_wstring(m_iRoyal_Tart_Count); // 문자열에 상수 더하기

			m_szObjName = m_wstObjName.c_str();	// wstring -> conat wchar*

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iRoyal_Tart_Count++;
		}

		if (1 == iObjNum)
		{
			fFoodInfo.eFoodKind = fFoodInfo.BURRITO;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstObjName = L"Burrito__";
			m_wstObjName += to_wstring(m_iBurrito_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
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
		//str.assign(m_wstObjName.begin(), m_wstObjName.end());

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();

		WinExec("notepad.exe ../../Data/Food.txt", SW_SHOW);
	}
#pragma endregion Food
}

void CLevel_GamePlay::ImGuiCoin()
{
	const _char* szObjName[] = { "CoinBronze", "CoinSilver", "CoinGold" };
	static int iObjNum = 0;
	ImGui::Combo("##2_COIN", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCoin::COININFO		tCoinInfo;
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();


	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z }; 

		if (0 == iObjNum)
		{
			tCoinInfo.fPos = m_f3ClickPos;
			tCoinInfo.eState = tCoinInfo.IDLE;
			tCoinInfo.eCoinKind = tCoinInfo.COIN_BRONZE;

			m_wstObjName = L"CoinBronze__";
			m_wstObjName += to_wstring(m_iCoinBronze_Count);

			m_szObjName = m_wstObjName.c_str();	

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinBronze_Count++;
		}
		else if (1 == iObjNum)
		{
			tCoinInfo.fPos = m_f3ClickPos;
			tCoinInfo.eState = tCoinInfo.IDLE;
			tCoinInfo.eCoinKind = tCoinInfo.COIN_SILVER;

			m_wstObjName = L"CoinSilver__";
			m_wstObjName += to_wstring(m_iCoinSilver_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinSilver_Count++;
		}
		else if (2 == iObjNum)
		{
			tCoinInfo.fPos = m_f3ClickPos;
			tCoinInfo.eState = tCoinInfo.IDLE;
			tCoinInfo.eCoinKind = tCoinInfo.COIN_GOLD;

			m_wstObjName = L"CoinGold__";
			m_wstObjName += to_wstring(m_iCoinGold_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinGold_Count++;
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

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();

		WinExec("notepad.exe ../../Data/Coin.txt", SW_SHOW);
	}
}

void CLevel_GamePlay::ImGuiPage()
{
	const _char* szObjName[] = { "Page_1" };
	static int iObjNum = 0;
	ImGui::Combo("##2_PAGE", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CPage::PAGEINFO		tPageInfo;
	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();


	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z };

		if (0 == iObjNum)
		{
			tPageInfo.fPos = m_f3ClickPos;

			m_wstObjName = L"Page_1__";
			m_wstObjName += to_wstring(m_iPage_1_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
				return;

			m_iPage_1_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Page Save"))
	{
		wofstream fout("../../Data/Page.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();

		WinExec("notepad.exe ../../Data/Page.txt", SW_SHOW);
	}
}

void CLevel_GamePlay::ImGuiMonster()
{
	const _char* szObjName[] = { "PigWarrior_BEE", "Pigs_COWBOY" };
	static int iObjNum = 0;
	ImGui::Combo("##2_MONSTER", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		CM_Monster::MONSTERDESC		tMonsterDesc;
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z };
		
		if (0 == iObjNum)
		{
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"PigWarrior_BEE__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_M_PigWarrior_BEE"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (1 == iObjNum)
		{
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Pigs_COWBOY__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_M_Pigs_COWBOY"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}

	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Monster Save"))
	{
		wofstream fout("../../Data/Monster.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();

		WinExec("notepad.exe ../../Data/Monster.txt", SW_SHOW);
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

			m_wstObjName = L"Royal_Tart__";
			m_wstObjName += to_wstring(i);

			wstring wstFoodNameTemp(pObjInfo.ObjName); // tchar -> wstring

			if (m_wstObjName == wstFoodNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return;
			}
		}

		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tFoodInfo.eFoodKind = tFoodInfo.BURRITO;
			tFoodInfo.fPos = pObjInfo.ObjPos;

			m_wstObjName = L"Burrito__";
			m_wstObjName += to_wstring(i);

			wstring wstFoodNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstFoodNameTemp)
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
	_int iCoinVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iCoinVecCount; i++)
		{
			tCoinInfo.fPos = pObjInfo.ObjPos;
			tCoinInfo.eState = tCoinInfo.IDLE;
			tCoinInfo.eCoinKind = tCoinInfo.COIN_BRONZE;

			m_wstObjName = L"CoinBronze__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return;
			}
		}

		for (_int i = 0; i < iCoinVecCount; i++)
		{
			tCoinInfo.fPos = pObjInfo.ObjPos;
			tCoinInfo.eState = tCoinInfo.IDLE;
			tCoinInfo.eCoinKind = tCoinInfo.COIN_SILVER;

			m_wstObjName = L"CoinSilver__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return;
			}
		}

		for (_int i = 0; i < iCoinVecCount; i++)
		{
			tCoinInfo.fPos = pObjInfo.ObjPos;
			tCoinInfo.eState = tCoinInfo.IDLE;
			tCoinInfo.eCoinKind = tCoinInfo.COIN_GOLD;

			m_wstObjName = L"CoinGold__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_GamePlay::PageLoad()
{
	wifstream		fin("../../Data/Page.txt", ios::in);

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

		CDataManager::GetInstance()->Set_PageInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CPage::PAGEINFO					tPageInfo;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_PageInfo();
	_int iFoodVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tPageInfo.fPos = pObjInfo.ObjPos;

			m_wstObjName = L"Page_1__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
					return;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_GamePlay::MonsterLoad()
{
	wifstream		fin("../../Data/Monster.txt", ios::in);

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

		CDataManager::GetInstance()->Set_PageInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CM_Monster::MONSTERDESC		tMonsterDesc;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_PageInfo();
	_int iMonsterVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"PigWarrior_BEE__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_PigWarrior_BEE"), &tMonsterDesc)))
					return;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Pigs_COWBOY__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Pigs_COWBOY"), &tMonsterDesc)))
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

