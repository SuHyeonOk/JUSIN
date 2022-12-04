#include "stdafx.h"
#include "..\public\ItemManager.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "Coin.h"

IMPLEMENT_SINGLETON(CItemManager)

CItemManager::CItemManager()
{
}

HRESULT CItemManager::RandomCoin_Clone(_float3 f3StartPos, _uint iBronzeCount, _uint iSilverCount, _uint iGoldCount)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCoin::COININFO					tCoinInfo;
	CObj_Manager::PLAYERINFO	ePlayerInfo;
	ePlayerInfo = CObj_Manager::GetInstance()->Get_Current_Player(); // ePlayerInfo.ePlayer_Level ÇöÀç Level

	for (_uint i = 0; i < iBronzeCount; i++)
	{
		tCoinInfo.fPos = f3StartPos;
		tCoinInfo.eState = tCoinInfo.COMEOUT;
		tCoinInfo.eCoinKind = tCoinInfo.COIN_BRONZE;

		m_wstObjName = L"CoinBronze_";
		m_wstObjName += to_wstring(i);

		m_szObjName = m_wstObjName.c_str(); // wstring -> conat wchar*

		if (FAILED(pGameInstance->Clone_GameObject(ePlayerInfo.ePlayer_Level, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
			return E_FAIL;
	}

	for (_uint i = 0; i < iSilverCount; i++)
	{
		tCoinInfo.fPos = f3StartPos;
		tCoinInfo.eState = tCoinInfo.COMEOUT;
		tCoinInfo.eCoinKind = tCoinInfo.COIN_SILVER;

		m_wstObjName = L"CoinSilver_";
		m_wstObjName += to_wstring(i);

		m_szObjName = m_wstObjName.c_str(); 

		if (FAILED(pGameInstance->Clone_GameObject(ePlayerInfo.ePlayer_Level, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
			return E_FAIL;
	}

	for (_uint i = 0; i < iGoldCount; i++)
	{
		tCoinInfo.fPos = f3StartPos;
		tCoinInfo.eState = tCoinInfo.COMEOUT;
		tCoinInfo.eCoinKind = tCoinInfo.COIN_GOLD;

		m_wstObjName = L"CoinGold_";
		m_wstObjName += to_wstring(i);

		m_szObjName = m_wstObjName.c_str(); 

		if (FAILED(pGameInstance->Clone_GameObject(ePlayerInfo.ePlayer_Level, m_szObjName, TEXT("Prototype_GameObject_Coin"), &tCoinInfo)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CItemManager::Free()
{
}