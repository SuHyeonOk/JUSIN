#include "stdafx.h"
#include "..\public\Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "Obj_Manager.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Skeleton.h"
#include "Level_Skeleton_Boss.h"
#include "Level_MiniGame.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Later_Loaing(TEXT("Layer_Loaing"))))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	/* eNextLevelID를 위한 자원을 쓰레드에게 준비해라. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/* 로딩씬 구성을 위한 객체를 생성한다. */
	/* 로딩바. 배경. 등등등. */

	return S_OK;
}

void CLevel_Loading::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CLevel_Loading::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);


	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == m_pLoader->isFinished())
	{
		CLevel*		pLevel = nullptr;

	
		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			CObj_Manager::GetInstance()->Set_Current_Level(LEVEL_LOGO);
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;

		case LEVEL_GAMEPLAY:
			CObj_Manager::GetInstance()->Set_Current_Level(LEVEL_GAMEPLAY);
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;

		case LEVEL_MINIGAME:
			CObj_Manager::GetInstance()->Set_Current_Level(LEVEL_MINIGAME);
			pLevel = CLevel_MiniGame::Create(m_pDevice, m_pContext);
			break;

		case LEVEL_SKELETON:
			CObj_Manager::GetInstance()->Set_Current_Level(LEVEL_SKELETON);
			pLevel = CLevel_Skleton::Create(m_pDevice, m_pContext);
			break;

		case LEVEL_SKELETON_BOSS:
			CObj_Manager::GetInstance()->Set_Current_Level(LEVEL_SKELETON_BOSS);
			pLevel = CLevel_Skleton_Boss::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;
		
		if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
			return;
	}

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Later_Loaing(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}
