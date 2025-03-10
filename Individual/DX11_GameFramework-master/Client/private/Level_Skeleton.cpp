#include "stdafx.h"
#include "..\public\Level_Skeleton.h"

#include <fstream> // @
#include "Imgui_PropertyEditor.h"	// @

#include "GameInstance.h"
#include "DataManager.h"
#include "Camera_Dynamic.h"
#include "Obj_Manager.h"
#include "Level_Loading.h"

#include "O_TextureObject.h"
#include "M_Monster.h"
#include "N_NPC.h"
#include "Food.h"
#include "Coin.h"
#include "Page.h"
#include "S_Jake_Son.h"
#include "E_FlyingEnvironment.h"
#include "Effect_Manager.h"

CLevel_Skleton::CLevel_Skleton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Skleton::Initialize()
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

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox_Skeleton"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Skeleton"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Penny()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Npc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Jake_Son(TEXT("Layer_JakeSon"))))
		return E_FAIL;

	// 파일 읽기
	Load_Food();
	Load_Coin();
	Load_Page();
	Load_Item();
	Load_Object();
	Load_Monster();
	Load_Envionment();

	return S_OK;
}

void CLevel_Skleton::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	ImGui(); // @ ImGui 를 사용하지 않을 때 주석!
	//ImGui_Light();
}

void CLevel_Skleton::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (CObj_Manager::GetInstance()->Get_NextLevel())
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SKELETON_BOSS))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CLevel_Skleton::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	//SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.75f, 0.75f, 0.75f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Load_Light();

	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;							// 포인트 조명
	//LightDesc.isEnable = true;
	//LightDesc.vPosition = _float4(-8.0f, 1.f, 5.3f, 1.f);          		// 위치
	//LightDesc.fRange = 2.5f;											// 조명 거리
	//LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.0f, 0.0f);				// 색상
	//LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 0.0f);				// 세기
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_PreviousData()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform * pObjTransformCom;
	pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(-5.f, 0.f, 6.f));

	pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(-6.f, 0.f, 6.f));

	pObjTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Camera"), TEXT("Com_Transform"), 0));
	pObjTransformCom->Set_Pos(_float3(-5.f, 3.7f, 1.f));

	CNavigation * pObjNavigationCom = nullptr;
	pObjNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), TEXT("Com_Navigation"), 0));	
	pObjNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton.txt"));
	pObjNavigationCom->Set_CellIndex(2);

	pObjNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), TEXT("Com_Navigation"), 0));
	pObjNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton.txt"));
	pObjNavigationCom->Set_CellIndex(2);

	pObjNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_BackGround"), TEXT("Com_Navigation"), 0));
	pObjNavigationCom->Ready_NextLevel(TEXT("../../Data/Navi_Skeleton.txt"));

	CObj_Manager::GetInstance()->Set_NextLevel(false);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Penny()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CM_Monster::MONSTERDESC	tMonsterDesc;
	tMonsterDesc.eMonsterKind = tMonsterDesc.PENNY;
	tMonsterDesc.f3Pos = _float3(1.32559f, 0.0f, 59.3413f);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_Penny"), TEXT("Prototype_GameObject_M_Penny"), &tMonsterDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Npc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CN_NPC::NPCDESC					tNpcDesc;
	tNpcDesc.eNpcType = tNpcDesc.DOCTOR;
	tNpcDesc.TransformDesc.f3Pos = _float3(-36.f, 0.f, 12.f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_Doctor__0"), TEXT("Prototype_GameObject_N_Doctor"), &tNpcDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Layer_GooseShop"), TEXT("Prototype_GameObject_N_GooseShop"), &_float3(-16.2501f, 0.0f, 66.1685f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Terrain"), &_float3(-50.f, 0.f, -20.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Dynamic::CAMERAINFO eCameraInfo;
	eCameraInfo.eLevel = LEVEL_SKELETON;
	eCameraInfo.f3Pos = _float3(-5.f, 0.f, 1.f);
	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &eCameraInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Finn(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Finn"), &_float3(-5.f, 0.f, 6.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Jake(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), pLayerTag, TEXT("Prototype_GameObject_Jake"), &_float3(-6.f, 0.f, 6.f))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Jake_Son(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CS_Jake_Son::JAKESONINFO tJakeSonInfo;
	tJakeSonInfo.eJakeSon = CS_Jake_Son::JAKESONINFO::JAKESON::JAKE_SON_D;
	tJakeSonInfo.fPos = _float3(-27.0f, 0.2f, 10.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pLayerTag, TEXT("Prototype_GameObject_S_Jake_Son"), &tJakeSonInfo)))
		return E_FAIL;

	tJakeSonInfo.eJakeSon = CS_Jake_Son::JAKESONINFO::JAKESON::JAKE_SON_E;
	tJakeSonInfo.fPos = _float3(17.0f, 0.2f, 53.0f);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pLayerTag, TEXT("Prototype_GameObject_S_Jake_Son"), &tJakeSonInfo)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pLayerTag, TEXT("Prototype_GameObject_Map_Skeleton"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel_Skleton::ImGui()
{
	ImGui::Begin("GamePlayTool");

	const _char* ItmeName[] = { "Empty", "Food", "Coin", "Page", "Item", "Object", "Monster", "Envionmen" };
	static int iItemNum = 0;
	ImGui::Combo("##2", &iItemNum, ItmeName, IM_ARRAYSIZE(ItmeName));

	if (1 == iItemNum)
		ImGui_Food();
	else if (2 == iItemNum)
		ImGui_Coin();
	else if (3 == iItemNum)
		ImGui_Page();
	else if (4 == iItemNum)
		ImGui_Item();
	else if (5 == iItemNum)
		ImGui_Object();
	else if (6 == iItemNum)
		ImGui_Monster();
	else if (7 == iItemNum)
		ImGui_Envionment();

	ImGui::End();

	return;
}

void CLevel_Skleton::ImGui_Light()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

		if (FAILED(pGameInstance->Set_Light(m_pDevice, m_pContext, LightDesc)))
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

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
			return;
	}

	if (ImGui::Button("Light Save"))
	{
		wofstream fout("../../Data/Skeleton_Light.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << arrfPosition[0] << L"|" << arrfPosition[1] << L"|" << arrfPosition[2] << "\n";

		fout.close();
	}

	if (ImGui::Button("Light txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Light.txt", SW_SHOW);

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_Skleton::ImGui_Food()
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Food"), &fFoodInfo)))
				return;

			m_iBurrito_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Food Save"))
	{
		wofstream fout("../../Data/Skeleton_Food.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Food.txt", SW_SHOW);
#pragma endregion Food
}

void CLevel_Skleton::ImGui_Coin()
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
				return;

			m_iCoinGold_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Coin Save"))
	{
		wofstream fout("../../Data/Skeleton_Coin.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Coin.txt", SW_SHOW);
}

void CLevel_Skleton::ImGui_Page()
{
	const _char* szObjName[] = { "Page_Paint","Page_Marcelint", "Page_Coin", "Page_Fiona" };
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
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::PAINT;

			m_wstObjName = L"Page_Paint__";
			m_wstObjName += to_wstring(m_iPage_1_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
				return;

			m_iPage_1_Count++;
		}
		if (1 == iObjNum)
		{
			tPageInfo.fPos = m_f3ClickPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT;

			m_wstObjName = L"Page_Marcelint__";
			m_wstObjName += to_wstring(m_iPage_1_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
				return;

			m_iPage_1_Count++;
		}
		if (2 == iObjNum)
		{
			tPageInfo.fPos = m_f3ClickPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::COIN;

			m_wstObjName = L"Page_Coin__";
			m_wstObjName += to_wstring(m_iPage_1_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
				return;

			m_iPage_1_Count++;
		}
		if (3 == iObjNum)
		{
			tPageInfo.fPos = m_f3ClickPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::FIONA;

			m_wstObjName = L"Page_Fiona__";
			m_wstObjName += to_wstring(m_iPage_1_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
				return;

			m_iPage_1_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Page Save"))
	{
		wofstream fout("../../Data/Skeleton_Page.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Page.txt", SW_SHOW);
}

void CLevel_Skleton::ImGui_Item()
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Key"), &m_f3ClickPos)))
				return;

			m_iItem_Count++;
		}
		else if (1 == iObjNum)	// 2D 오브젝트
		{
			m_wstObjName = L"Layer_Heart__";
			m_wstObjName += to_wstring(m_iItem_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_Heart"), &m_f3ClickPos)))
				return;

			m_iItem_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Item Save"))
	{
		wofstream fout("../../Data/Skeleton_Item.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Item.txt", SW_SHOW);
}

void CLevel_Skleton::ImGui_Object()
{
	const _char* szObjName[] = { "Box", "Portal", "BearTrap", "PortalOff", "JakeSon_Test" };
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_O_Box"), &m_f3ClickPos)))
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

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_O_TextureObject"), &tTextureObject)))
				return;

			m_iObject_Count++;
		}
		else if (2 == iObjNum)
		{
			m_wstObjName = L"Layer_BearTrap__";
			m_wstObjName += to_wstring(m_iObject_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_O_BearTrap"), &m_f3ClickPos)))
				return;

			m_iObject_Count++;
		}
		else if (3 == iObjNum)
		{
			m_wstObjName = L"Layer_PortalOff__";
			m_wstObjName += to_wstring(m_iObject_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_O_PortalOff"), &m_f3ClickPos)))
				return;

			m_iObject_Count++;
		}
		else if (4 == iObjNum)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CS_Jake_Son::JAKESONINFO tJakeSonInfo;
			tJakeSonInfo.eJakeSon = CS_Jake_Son::JAKESONINFO::JAKESON::JAKE_SON_E;
			tJakeSonInfo.fPos = m_f3ClickPos;
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, TEXT("Prototype_GameObject_S_Jake_Son"), TEXT("Prototype_GameObject_S_Jake_Son"), &tJakeSonInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);

		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Object Save"))
	{
		wofstream fout("../../Data/Skeleton_Object.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Object.txt", SW_SHOW);
}

void CLevel_Skleton::ImGui_Monster()
{
	const _char* szObjName[] = { "Ghost_1", "Ghost_2", "Ghost_3", 
		"Skeleton_Archer_1", "Skeleton_Archer_2", 
		"Skeleton_Shield_1", "Skeleton_Shield_2", "Penny" };
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
			tMonsterDesc.eMonsterKind = tMonsterDesc.GHOST_1;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Ghost_1__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Ghost"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (1 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.GHOST_2;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Ghost_2__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Ghost"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (2 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.GHOST_3;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Ghost_3__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Ghost"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}

		if (3 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_ARCHER_1;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Skeleton_Archer_1__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (4 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_ARCHER_2;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Skeleton_Archer_2__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}

		if (5 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_SHIELD_1;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Skeleton_Shield_1__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Skeleton_Shield"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (6 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_SHIELD_2;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Skeleton_Shield_2__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Skeleton_Shield"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
		if (7 == iObjNum)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.PENNY;
			tMonsterDesc.f3Pos = m_f3ClickPos;

			m_wstObjName = L"Layer_Penny__";
			m_wstObjName += to_wstring(m_iMonster_Count);

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_M_Penny"), &tMonsterDesc)))
				return;

			m_iMonster_Count++;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Monster Save"))
	{
		wofstream fout("../../Data/Skeleton_Monster.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if(ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Monster.txt", SW_SHOW);
}

void CLevel_Skleton::ImGui_Envionment()
{
	const _char* szObjName[] = { "CnaFire_Big", "CanFire_Medium", "CanFire_Small", "Can_Smoke", "FireSparks", "FireSparks_Small", "Butterflies_Luminous" };
	static int iObjNum = 0;
	ImGui::Combo("##2_Envionmen", &iObjNum, szObjName, IM_ARRAYSIZE(szObjName));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4		f4MousePos;
	f4MousePos = pGameInstance->Get_MousePos();

	CE_FlyingEnvironment::EFFECTINFO tEffectInfo;

	static _float fObjectY;
	ImGui::InputFloat("Object Y", &fObjectY);

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
	{
		m_f3ClickPos = { f4MousePos.x, fObjectY, f4MousePos.z };

		if (0 == iObjNum)
		{
			m_wstObjName = L"CnaFire_Big";

			tEffectInfo.eType = CE_FlyingEnvironment::CANFIRE_BIG;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;
		}
		if (1 == iObjNum)
		{
			m_wstObjName = L"CanFire_Medium";

			tEffectInfo.eType = CE_FlyingEnvironment::CANFIRE_MEDIUM;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;
		}
		if (2 == iObjNum)
		{
			m_wstObjName = L"CanFire_Small";

			tEffectInfo.eType = CE_FlyingEnvironment::CANFIRE_SMALL;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;
		}
		if (3 == iObjNum)
		{
			m_wstObjName = L"Can_Smoke";

			CEffect_Manager::GetInstance()->Effect_Can_Smoke(m_f3ClickPos);
		}
		if (4 == iObjNum)
		{
			m_wstObjName = L"FireSparks";

			tEffectInfo.eType = CE_FlyingEnvironment::FIRESPARKS;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();
			
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;
		}
		if (5 == iObjNum)
		{
			m_wstObjName = L"FireSparks_Small";
			
			tEffectInfo.eType = CE_FlyingEnvironment::FIRESPARKS_SMALL;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();
			
			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;
		}
		if (6 == iObjNum)
		{
			m_wstObjName = L"Butterflies_Luminous";
			
			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_LUMINOUS;
			tEffectInfo.f3Pos = m_f3ClickPos;

			m_szObjName = m_wstObjName.c_str();

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, m_szObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	if (ImGui::Button("Envionmen Save"))
	{
		wofstream fout("../../Data/Skeleton_Envionment.txt", ios::out | ios::app);
		if (fout.fail())
		{
			MSG_BOX("Failed to Save File");
			return;
		}

		fout << m_wstObjName << "|" << m_f3ClickPos.x << "|" << m_f3ClickPos.y << L"|" << m_f3ClickPos.z << "\n";

		fout.close();
	}

	if (ImGui::Button("Data_txt"))
		WinExec("notepad.exe ../../Data/Skeleton_Envionment.txt", SW_SHOW);
}

HRESULT CLevel_Skleton::Load_Light()
{
	wifstream		fin("../../Data/Skeleton_Light.txt", ios::in);

	if (fin.fail())
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
	}

	_tchar szObjPosX[MAX_PATH] = L"";
	_tchar szObjPosY[MAX_PATH] = L"";
	_tchar szObjPosZ[MAX_PATH] = L"";

	_float	fObjPosX = 0.f;
	_float	fObjPosY = 0.f;
	_float	fObjPosZ = 0.f;

	while (true)
	{
		fin.getline(szObjPosX, MAX_PATH, '|');
		fin.getline(szObjPosY, MAX_PATH, '|');
		fin.getline(szObjPosZ, MAX_PATH);

		if (fin.eof())
			break;

		fObjPosX = (_float)_tstof(szObjPosX);
		fObjPosY = (_float)_tstof(szObjPosY);
		fObjPosZ = (_float)_tstof(szObjPosZ);

		m_vecfloat3.push_back(_float3(fObjPosX, fObjPosY, fObjPosZ));
	}

	for (auto& pLightInfo : m_vecfloat3)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHTDESC::TYPE_POINT;											// 포인트 조명
		LightDesc.isEnable = true;
		LightDesc.vPosition = _float4(pLightInfo.x, pLightInfo.y, pLightInfo.z, 0.0f);
		LightDesc.fRange = 2.5f;															// 조명 거리
		LightDesc.vDiffuse = _float4(0.9f, 0.3f, 0.0f, 0.0f);								// 색상
		LightDesc.vAmbient = _float4(0.2f, 0.0f, 0.0f, 0.0f);								// 세기
		LightDesc.vSpecular = LightDesc.vDiffuse;

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Food()
{
	wifstream		fin("../../Data/Skeleton_Food.txt", ios::in);

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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Food"), &tFoodInfo)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Coin()
{
	wifstream		fin("../../Data/Skeleton_Coin.txt", ios::in);

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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Page()
{
	wifstream		fin("../../Data/Skeleton_Page.txt", ios::in);

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

	CPage::PAGEINFO					tPageInfo;
	vector<CDataManager::OBJINFO>	eVecObjInfo = CDataManager::GetInstance()->Get_PageInfo();
	_int iPageVecCount = _int(eVecObjInfo.size());

	for (auto& pObjInfo : eVecObjInfo)
	{
		for (_int i = 0; i < iPageVecCount; i++)
		{
			tPageInfo.fPos = pObjInfo.ObjPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::PAINT;

			m_wstObjName = L"Page_Paint__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iPageVecCount; i++)
		{
			tPageInfo.fPos = pObjInfo.ObjPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::MARCELINT;

			m_wstObjName = L"Page_Marcelint__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iPageVecCount; i++)
		{
			tPageInfo.fPos = pObjInfo.ObjPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::COIN;

			m_wstObjName = L"Page_Coin__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iPageVecCount; i++)
		{
			tPageInfo.fPos = pObjInfo.ObjPos;
			tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL::FIONA;

			m_wstObjName = L"Page_Fiona__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Page"), &tPageInfo)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Item()
{
	wifstream		fin("../../Data/Skeleton_Item.txt", ios::in);

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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Key"), &pObjInfo.ObjPos)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_Heart"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Object()
{
	wifstream		fin("../../Data/Skeleton_Object.txt", ios::in);

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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_Box"), &pObjInfo.ObjPos)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_TextureObject"), &tTextureObject)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_BearTrap"), &pObjInfo.ObjPos)))
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
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_O_PortalOff"), &pObjInfo.ObjPos)))
					return E_FAIL;
			}
		}
	}
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Monster()
{
	wifstream		fin("../../Data/Skeleton_Monster.txt", ios::in);

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
			tMonsterDesc.eMonsterKind = tMonsterDesc.GHOST_1;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Ghost_1__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Ghost"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.GHOST_2;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Ghost_2__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Ghost"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.GHOST_3;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Ghost_3__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Ghost"), &tMonsterDesc)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_ARCHER_1;
			tMonsterDesc.f3Pos = _float3(pObjInfo.ObjPos.x, pObjInfo.ObjPos.y, pObjInfo.ObjPos.z);

			m_wstObjName = L"Layer_Skeleton_Archer_1__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_ARCHER_2;
			tMonsterDesc.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Layer_Skeleton_Archer_2__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Skeleton_Archer"), &tMonsterDesc)))
					return E_FAIL;
			}
		}

		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_SHIELD_1;
			tMonsterDesc.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Layer_Skeleton_Shield_1__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Skeleton_Shield"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
		for (_int i = 0; i < iMonsterVecCount; i++)
		{
			tMonsterDesc.eMonsterKind = tMonsterDesc.SKELETON_SHIELD_2;
			tMonsterDesc.f3Pos = pObjInfo.ObjPos;

			m_wstObjName = L"Layer_Skeleton_Shield_2__";
			m_wstObjName += to_wstring(i);

			wstring wstObjNameTemp(pObjInfo.ObjName);

			if (m_wstObjName == wstObjNameTemp)
			{
				if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_M_Skeleton_Shield"), &tMonsterDesc)))
					return E_FAIL;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Skleton::Load_Envionment()
{
	wifstream		fin("../../Data/Skeleton_Envionment.txt", ios::in);

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

	for (auto& pObjInfo : eVecObjInfo)
	{
		wstring wstObjNameTemp(pObjInfo.ObjName);

		if (TEXT("CnaFire_Big") == wstObjNameTemp)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::CANFIRE_BIG;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return E_FAIL;
		}
		else if (TEXT("CanFire_Medium") == wstObjNameTemp)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::CANFIRE_MEDIUM;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return E_FAIL;
		}
		else if (TEXT("CanFire_Small") == wstObjNameTemp)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::CANFIRE_SMALL;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return E_FAIL;
		}
		else if (TEXT("Can_Smoke") == wstObjNameTemp)
		{
			CEffect_Manager::GetInstance()->Effect_Can_Smoke(pObjInfo.ObjPos);
		}
		else if (TEXT("FireSparks") == wstObjNameTemp)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::FIRESPARKS;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return E_FAIL;
		}
		else if (TEXT("FireSparks_Small") == wstObjNameTemp)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::FIRESPARKS_SMALL;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return E_FAIL;
		}
		else if (TEXT("Butterflies_Luminous") == wstObjNameTemp)
		{
			tEffectInfo.eType = CE_FlyingEnvironment::BUTTERFLIES_LUMINOUS;
			tEffectInfo.f3Pos = pObjInfo.ObjPos;

			if (FAILED(pGameInstance->Clone_GameObject(LEVEL_SKELETON, pObjInfo.ObjName, TEXT("Prototype_GameObject_E_FlyingEnvironment"), &tEffectInfo)))
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Skleton * CLevel_Skleton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Skleton*		pInstance = new CLevel_Skleton(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Skleton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Skleton::Free()
{
	__super::Free();


}

