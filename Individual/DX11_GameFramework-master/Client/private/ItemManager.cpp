#include "stdafx.h"
#include "..\public\ItemManager.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"
#include "Effect_Manager.h"

#include "Coin.h"
#include "Page.h"

IMPLEMENT_SINGLETON(CItemManager)

CItemManager::CItemManager()
{
}

HRESULT CItemManager::RandomCoin_Clone(_float3 f3StartPos, _uint iBronzeCount, _uint iSilverCount, _uint iGoldCount)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCoin::COININFO					tCoinInfo;

	for (_uint i = 0; i < iBronzeCount; i++)
	{
		tCoinInfo.fPos = f3StartPos;
		tCoinInfo.eState = tCoinInfo.COMEOUT;
		tCoinInfo.eCoinKind = tCoinInfo.COIN_BRONZE;

		m_wstObjName = L"Layer_CoinBronze_";
		m_wstObjName += to_wstring(i);

		m_szObjName = m_wstObjName.c_str(); // wstring -> conat wchar*

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
			return E_FAIL;
	}

	for (_uint i = 0; i < iSilverCount; i++)
	{
		tCoinInfo.fPos = f3StartPos;
		tCoinInfo.eState = tCoinInfo.COMEOUT;
		tCoinInfo.eCoinKind = tCoinInfo.COIN_SILVER;

		m_wstObjName = L"Layer_CoinSilver_";
		m_wstObjName += to_wstring(i);

		m_szObjName = m_wstObjName.c_str(); 

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
			return E_FAIL;
	}

	for (_uint i = 0; i < iGoldCount; i++)
	{
		tCoinInfo.fPos = f3StartPos;
		tCoinInfo.eState = tCoinInfo.COMEOUT;
		tCoinInfo.eCoinKind = tCoinInfo.COIN_GOLD;

		m_wstObjName = L"Layer_CoinGold_";
		m_wstObjName += to_wstring(i);

		m_szObjName = m_wstObjName.c_str(); 

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
			return E_FAIL;
	}

	for (_int i = 0; i < 3; i++)
		CEffect_Manager::GetInstance()->Effect_Coin_Create(f3StartPos);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CItemManager::RandomPage_Clone(_float3 f3StartPos, _uint iCount)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CPage::PAGEINFO tPageInfo;
	tPageInfo.fPos = _float3(f3StartPos.x, f3StartPos.y, f3StartPos.z);
	tPageInfo.ePlayerSkill = CSkill_Manager::PLAYERSKILL::SKILL(CUtilities_Manager::GetInstance()->Get_Random(0, 3));
	tPageInfo.bJemp = true;

	for (_uint i = 0; i < iCount; ++i)
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Page_Monster"), TEXT("Prototype_GameObject_Page"), &tPageInfo)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CItemManager::Free()
{
}