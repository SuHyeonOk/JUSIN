#include "stdafx.h"
#include "..\public\Level_MiniGame.h"

#include <fstream> // @
#include "Imgui_PropertyEditor.h"	// @

#include "GameInstance.h"
#include "DataManager.h"
#include "Camera_Dynamic.h"
#include "Level_Loading.h"

#include "Obj_Manager.h"
#include "O_TextureObject.h"
#include "M_Monster.h"
#include "Food.h"
#include "Coin.h"
#include "O_Collider.h"
#include "E_FlyingEnvironment.h"

#include "MiniGmae_Collider.h"
#include "Cake.h"

CLevel_MiniGame::CLevel_MiniGame(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_MiniGame::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Game()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox_MiniGmae"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_MiniGame"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_FinnAndJake(TEXT("Layer_FinnAndJake"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	CObj_Manager::GetInstance()->Set_NextLevel(false);

	// 파일 읽기
	//Load_Food();
	//Load_Coin();
	//Load_Item();
	//Load_Object();
	//Load_Monster();
	//Load_Envionment();
	Load_MiniGame();

	return S_OK;
}

void CLevel_MiniGame::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	ImGui(); // @ ImGui 를 사용하지 않을 때 주석!

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_N))
	{
		// 무지개콘
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Lady_Rainicorn"), TEXT("Prototype_GameObject_Lady_Rainicorn"), &_float3(62.8778f, 0.0f, 27.6776f))))
			return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_MiniGame::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (CObj_Manager::GetInstance()->Get_NextLevel())
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_MiniGame::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//SetWindowText(g_hWnd, TEXT("Level : GMAE_MINIGMAE"));

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);

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

HRESULT CLevel_MiniGame::Ready_Layer_Game()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 콜라이더
	CMiniGame_Collider::COLLIDERINFO ColliderIndo;
	ColliderIndo.eType = CMiniGame_Collider::KNIVESRAIN;
	ColliderIndo.f3Pos = _float3(4.79107f, 0.0f, 9.38578f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_MiniGame_Collider"), TEXT("Prototype_GameObject_MiniGame_Collider"), &ColliderIndo)))
		return E_FAIL;

	// 무지개콘
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Lady_Rainicorn"), TEXT("Prototype_GameObject_Lady_Rainicorn"), &_float3(62.8778f, 0.0f, 27.6776f))))
		return E_FAIL;

	// 달팽이
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Snail"), TEXT("Prototype_GameObject_Snail"), &_float3(-6.0f, 0.0f, -20.0f))))
		return E_FAIL;

	// 비모 위치 84.6781 | 0 | 12.0082

	// 케이크
	CCake::OBJECTINFO tObjectInfo;
	tObjectInfo.eType = CCake::BLUE;
	tObjectInfo.f3Position = { 23.0f, 0.0f, 16.0f };
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Ckae_Blue"), TEXT("Prototype_GameObject_Ckae"), &tObjectInfo)))
		return E_FAIL;

	tObjectInfo.eType = CCake::MAGENTA;
	tObjectInfo.f3Position = { 24.0f, 0.0f, 16.0f };
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Ckae_Magenta"), TEXT("Prototype_GameObject_Ckae"), &tObjectInfo)))
		return E_FAIL;

	tObjectInfo.eType = CCake::YELLOW;
	tObjectInfo.f3Position = { 25.0f, 0.0f, 16.0f };
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Ckae_Yellow"), TEXT("Prototype_GameObject_Ckae"), &tObjectInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Terrain"), &_float3(-50.f, 0.f, -20.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

#ifdef F2_SKELETON
	CCamera_Dynamic::CAMERAINFO eCameraInfo;
	eCameraInfo.eLevel = LEVEL_MINIGAME;
	eCameraInfo.f3Pos = _float3(-5.0f, 3.7f, -20.0f);
	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &eCameraInfo)))
		return E_FAIL;
#else
	CTransform * pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(-5.0f, 3.7f, -20.0f));
#endif

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_FinnAndJake(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_FinnAndJake"), TEXT("Prototype_GameObject_FinnAndJake"), &_float3(-4.0f, 0.0f, -20.0f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pLayerTag, TEXT("Prototype_GameObject_Map_MiniGame"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel_MiniGame::ImGui()
{
	ImGui::Begin("GamePlayTool");

	const _char* ItmeName[] = { "Empty", "Food", "Coin", "Item", "Object", "Monster", "Envionmen", "Game" };
	static int iItemNum = 0;
	ImGui::Combo("##2", &iItemNum, ItmeName, IM_ARRAYSIZE(ItmeName));

	if (1 == iItemNum)
		ImGui_Food();
	else if (2 == iItemNum)
		ImGui_Coin();
	else if (3 == iItemNum)
		ImGui_Item();
	else if (4 == iItemNum)
		ImGui_Object();
	else if (5 == iItemNum)
		ImGui_Monster();
	else if (6 == iItemNum)
		ImGui_Envionment();
	else if (7 == iItemNum)
		ImGui_MiniGame();

	ImGui::End();

	return;
}

void CLevel_MiniGame::ImGui_Food()
{
#pragma region Food
	const _char* szObjName[] = { "Apple_Pie", "Royal_Tart", "Burrito" };
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
			fFoodInfo.eFoodKind = fFoodInfo.APPLE_PIE;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstObjName = L"Apple_Pie__";
			m_wstObjName += to_wstring(m_iRoyal_Tart_Count); // 문자열에 상수 더하기

			m_szObjName = m_wstObjName.c_str();	// wstring -> conat wchar*

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iRoyal_Tart_Count++;
		}

		if (1 == iObjNum)
		{
			fFoodInfo.eFoodKind = fFoodInfo.ROYAL_TART;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstObjName = L"Royal_Tart__";
			m_wstObjName += to_wstring(m_iRoyal_Tart_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iRoyal_Tart_Count++;
		}

		if (2 == iObjNum)
		{
			fFoodInfo.eFoodKind = fFoodInfo.BURRITO;
			fFoodInfo.fPos = m_f3ClickPos;

			m_wstObjName = L"Burrito__";
			m_wstObjName += to_wstring(m_iBurrito_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iBurrito_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Food Save"))
	{
		wofstream fout("../../Data/MiniGmae_Food.txt", ios::out | ios::app);
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
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGmae_Food.txt", SW_SHOW);
#pragma endregion Food
}

void CLevel_MiniGame::ImGui_Coin()
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinGold_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Coin Save"))
	{
		wofstream fout("../../Data/MiniGmae_Coin.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGmae_Coin.txt", SW_SHOW);
}

void CLevel_MiniGame::ImGui_Item()
{
	const _char* szObjName[] = { "Key", "Heart" };
	static int iObjNum = 0;
	ImGui::Combo("##2_Item", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z };

		if (0 == iObjNum)
		{
			m_wstObjName = L"Layer_Key__";
			m_wstObjName += to_wstring(m_iItem_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Key"), &m_f3ClickPos)))
				return;

			m_iItem_Count++;
		}
		else if (1 == iObjNum)	// 2D 오브젝트
		{
			m_wstObjName = L"Layer_Heart__";
			m_wstObjName += to_wstring(m_iItem_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_Heart"), &m_f3ClickPos)))
				return;

			m_iItem_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Item Save"))
	{
		wofstream fout("../../Data/MiniGame_Item.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGame_Item.txt", SW_SHOW);
}

void CLevel_MiniGame::ImGui_Object()
{
	const _char* szObjName[] = { "Box", "Portal", "BearTrap", "PortalOff" };
	static int iObjNum = 0;
	ImGui::Combo("##2_Object", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z };

		if (0 == iObjNum)
		{
			m_wstObjName = L"Layer_Box__";
			m_wstObjName += to_wstring(m_iObject_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_O_Box"), &m_f3ClickPos)))
				return;

			m_iObject_Count++;
		}
		else if (1 == iObjNum)	// 2D 오브젝트
		{
			CO_TextureObject::TEXTUREOBJECT		tTextureObject;
			tTextureObject.eTextureType = tTextureObject.PORTAL;
			tTextureObject.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Portal__";
			m_wstObjName += to_wstring(m_iObject_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_O_TextureObject"), &tTextureObject)))
				return;

			m_iObject_Count++;
		}
		else if (2 == iObjNum)
		{
			m_wstObjName = L"Layer_BearTrap__";
			m_wstObjName += to_wstring(m_iObject_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_O_BearTrap"), &m_f3ClickPos)))
				return;

			m_iObject_Count++;
		}
		else if (3 == iObjNum)
		{
			m_wstObjName = L"Layer_PortalOff__";
			m_wstObjName += to_wstring(m_iObject_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_O_PortalOff"), &m_f3ClickPos)))
				return;

			m_iObject_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Object Save"))
	{
		wofstream fout("../../Data/MiniGame_Object.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGame_Object.txt", SW_SHOW);
}

void CLevel_MiniGame::ImGui_Monster()
{
	const _char* szObjName[] = { 
		"PigWarrior_BEE", "PigWarrior_WORKER", 
		"Pigs_COWBOY", "Pigs_SR", 
		"Gronmes_BULE", "Gronmes_RED", "Gronmes_YELLOW", 
		"Tree_Witch", "Magic_Man", "Mimix" };
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
			tMonsterDesc.eMonsterKind = tMonsterDesc.W_BEE;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_PigWarrior_BEE__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_PigWarrior"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (1 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.W_WORKE;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_PigWarrior_WORKER__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_PigWarrior"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (2 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.S_COWBOY;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Pigs_COWBOY__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Pigs"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (3 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.S_SR;
			tMonsterDesc.f3Pos = m_f3ClickPos;
			
			m_wstObjName = L"Layer_Pigs_SR__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Pigs"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (4 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.G_BLUE;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Gronmes_BLUE__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Gronmes"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (5 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.G_RED;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Gronmes_RED__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Gronmes"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (6 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.G_YELLOW;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Gronmes_YELLOW__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Gronmes"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (7 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.TREE_WITCH;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Tree_Witch__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Tree_Witch"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (8 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.MAGIC_MAN;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Magic_Man__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Magic_Man"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (9 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.MAGIC_MAN;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Mimic__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_M_Mimic"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Monster Save"))
	{
		wofstream fout("../../Data/MiniGmae_Monster.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if(ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGmae_Monster.txt", SW_SHOW);
}

void CLevel_MiniGame::ImGui_Envionment()
{
	const _char* szObjName[] = { "Butterflies_Bule", "Butterflies_Red", "Butterflies_Yellow", "Cloud" };
	static int iObjNum = 0;
	ImGui::Combo("##2_Envionmen", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));

	static _float fObjectY;
	ImGui::InputFloat("ObjectY", &fObjectY);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	CE_FlyingEnvironment::EFFECTINFO tEffectInfo;

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, f4MousePos.y, f4MousePos.z };

		if (0 == iObjNum)
		{
			m_wstObjName = L"Butterflies_Bule";
			m_wstObjName += to_wstring(m_iItem_Count);

			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_BLUE;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;

			m_iItem_Count++;
		}
		else if (1 == iObjNum)
		{
			m_wstObjName = L"Butterflies_Red";
			m_wstObjName += to_wstring(m_iItem_Count);

			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_RED;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;

			m_iItem_Count++;
		}
		else if (2 == iObjNum)
		{
			m_wstObjName = L"Butterflies_Yellow";
			m_wstObjName += to_wstring(m_iItem_Count);

			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_YELLOW;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;

			m_iItem_Count++;
		}
		else if (3 == iObjNum)
		{		
			m_wstObjName = L"Cloud";
			
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Cloud"), TEXT("Prototype_GameObject_O_Cloud"), &_float3(m_f3ClickPos.x, fObjectY, m_f3ClickPos.z))))
				return;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Envionmen Save"))
	{
		wofstream fout("../../Data/MiniGmae_Envionment.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGmae_Envionment.txt", SW_SHOW);
}

void CLevel_MiniGame::ImGui_MiniGame()
{
	const _char* szObjName[] = { "Knives_Rain" };
	static int iObjNum = 0;
	ImGui::Combo("##2_MiniGame", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));

	static _float fObjectY;
	ImGui::InputFloat("ObjectY", &fObjectY);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	CE_FlyingEnvironment::EFFECTINFO tEffectInfo;

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, fObjectY, f4MousePos.z };

		if (0 == iObjNum)
		{
			m_wstObjName = L"Knives_Rain";
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Knives_Rain"), TEXT("Prototype_GameObject_Knives_Rain"), &m_f3ClickPos)))
				return;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Envionmen Save"))
	{
		wofstream fout("../../Data/MiniGmae.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/MiniGmae.txt", SW_SHOW);
}

HRESULT CLevel_MiniGame::Load_Food()
{
	wifstream		fin("../../Data/MiniGmae_Food.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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
			tFoodInfo.eFoodKind = tFoodInfo.APPLE_PIE;
			tFoodInfo.fPos = pObjInfo.ObjPos;

			m_wstObjName = L"Apple_Pie__";
			m_wstObjName += to_wstring(i);

			wstring wstFoodNameTemp(pObjInfo.ObjName); // tchar -> wstring

			if (m_wstObjName == wstFoodNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iFoodVecCount; i++)
		{
			tFoodInfo.eFoodKind = tFoodInfo.ROYAL_TART;
			tFoodInfo.fPos = pObjInfo.ObjPos;

			m_wstObjName = L"Royal_Tart__";
			m_wstObjName += to_wstring(i);

			wstring wstFoodNameTemp(pObjInfo.ObjName); // tchar -> wstring

			if (m_wstObjName == wstFoodNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return E_FAIL;
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Load_Coin()
{
	wifstream		fin("../../Data/MiniGmae_Coin.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return E_FAIL;
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return E_FAIL;
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Load_Item()
{
	wifstream		fin("../../Data/MiniGame_Item.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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

		CDataManager::GetInstance()->Set_ItemInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_ItemInfo();
	_int iVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iVecCount; i++)
		{
			m_wstObjName = L"Layer_Key__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Key"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iVecCount; i++)
		{
			m_wstObjName = L"Layer_Heart__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_Heart"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Load_Object()
{
	wifstream		fin("../../Data/MiniGame_Object.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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

		CDataManager::GetInstance()->Set_ObjectInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_ObjectInfo();
	_int iVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iVecCount; i++)
		{
			m_wstObjName = L"Layer_Box__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_Box"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iVecCount; i++)
		{
			CO_TextureObject::TEXTUREOBJECT		tTextureObject;
			tTextureObject.eTextureType = tTextureObject.PORTAL;
			tTextureObject.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Layer_Portal__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_TextureObject"), &tTextureObject)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iVecCount; i++)
		{
			m_wstObjName = L"Layer_BearTrap__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_BearTrap"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iVecCount; i++)
		{
			m_wstObjName = L"Layer_PortalOff__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_PortalOff"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Load_Monster()
{
	wifstream		fin("../../Data/MiniGmae_Monster.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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

		CDataManager::GetInstance()->Set_MonsterInfo(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CM_Monster::MONSTERDESC			tMonsterDesc;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_MonsterInfo();
	_int iMonsterVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.W_BEE;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_PigWarrior_BEE__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_PigWarrior"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.W_WORKE;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_PigWarrior_WORKER__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_PigWarrior"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.S_COWBOY;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Pigs_COWBOY__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Pigs"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.S_SR;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Pigs_SR__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Pigs"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.G_BLUE;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Gronmes_BLUE__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Gronmes"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.G_RED;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Gronmes_RED__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Gronmes"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.G_YELLOW;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Gronmes_YELLOW__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Gronmes"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.TREE_WITCH;	// 요기
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Tree_Witch__";	// 요기
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Tree_Witch"), &tMonsterDesc)))	// 요기
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.MAGIC_MAN;	// 요기
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Magic_Man__";	// 요기
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Magic_Man"), &tMonsterDesc)))	// 요기
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.MIMIC;	// 요기
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Mimic__";	// 요기
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Mimic"), &tMonsterDesc)))	// 요기
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Load_Envionment()
{
	wifstream		fin("../../Data/MiniGmae_Envionment.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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

		CDataManager::GetInstance()->Set_Environment(*szObjName, _float3(fObjPosX, fObjPosY, fObjPosZ));
	}



	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CE_FlyingEnvironment::EFFECTINFO tEffectInfo;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_EnvironmentInfo();
	_int iCoinVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iCoinVecCount; i++)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_BLUE;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Butterflies_Bule";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iCoinVecCount; i++)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_RED;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Butterflies_Red";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iCoinVecCount; i++)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_YELLOW;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Butterflies_Yellow";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
					return E_FAIL;
			}
		}
		wstring wstObjNameTemp(pObjInfo.ObjName);
		if (TEXT("Cloud") == wstObjNameTemp)
		{
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Cloud"), TEXT("Prototype_GameObject_O_Cloud"), &pObjInfo.ObjPos)))
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_MiniGame::Load_MiniGame()
{
	wifstream		fin("../../Data/MiniGmae.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
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

	CE_FlyingEnvironment::EFFECTINFO tEffectInfo;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_PageInfo();

	for (auto& pObjInfo : eVecObjInfo)
	{
		wstring wstObjNameTemp(pObjInfo.ObjName);

		if (TEXT("Knives_Rain") == wstObjNameTemp)
		{
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_MINIGAME, TEXT("Layer_Knives_Rain"), TEXT("Prototype_GameObject_Knives_Rain"), &pObjInfo.ObjPos)))
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_MiniGame * CLevel_MiniGame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_MiniGame*		pInstance = new CLevel_MiniGame(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_MiniGame::Free()
{
	__super::Free();

	CDataManager::GetInstance()->Free();
}

