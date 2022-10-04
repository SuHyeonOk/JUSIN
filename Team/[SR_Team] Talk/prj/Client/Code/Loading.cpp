#include "stdafx.h"
#include "..\Header\Loading.h"

#include "BulletMgr.h"
#include "Export_Function.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	,m_bFinish(false)
	,m_eID(LOADING_END)
{
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	m_eID = eID;
	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	// 그리기 컴포넌트
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColCom", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcColCom", CRcCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexCom", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTexCom", CCubeTex::Create(m_pGraphicDev)), E_FAIL);

	// 텍스처
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Ma.jpg", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4)), E_FAIL);
		
	// 컴포넌트
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformCom", CTransform::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorCom", CCalculator::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AnimatorCom", CAnimator::Create(m_pGraphicDev)), E_FAIL);

	// 플레이어 공격무기 - wand1 텍스쳐
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WandTexture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Wand/wand%d.png", TEX_NORMAL,3)), E_FAIL);
		
	// bulletmgr
	CBulletMgr::GetInstance()->Ready_Proto(m_pGraphicDev);

	// GreenSlime_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Idle/GreenSlime%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Attack/GreenSlime%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Hit/GreenSlime%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GreenSlimeDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/GreenSlime/Die/GreenSlime%d.png", TEX_NORMAL, 3)), E_FAIL);

	// PinkSlime_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Idle/PinkSlime%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Attack/PinkSlime%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Hit/PinkSlime%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PinkSlimeDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/PinkSlime/Die/PinkSlime%d.png", TEX_NORMAL, 12)), E_FAIL);

	// Fist_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FistIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Fist/Idle/Fist%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FistATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Fist/Attack/Fist%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FistHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Fist/Hit/Fist%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FistDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Fist/Die/Fist%d.png", TEX_NORMAL, 3)), E_FAIL);

	// BlueBat_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BlueBatIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BlueBat/Idle/BlueBat%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BlueBatATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BlueBat/Attack/BlueBat%d.png", TEX_NORMAL, 10)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BlueBatHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BlueBat/Hit/BlueBat%d.png", TEX_NORMAL, 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BlueBatDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BlueBat/Die/BlueBat%d.png", TEX_NORMAL, 3)), E_FAIL);
	
	// BrownBat_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownBatIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BrownBat/Idle/BrownBat%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownBatATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BrownBat/Attack/BrownBat%d.png", TEX_NORMAL, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownBatHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BrownBat/Hit/BrownBat%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownBatDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/BrownBat/Die/BrownBat%d.png", TEX_NORMAL, 2)), E_FAIL);

	////////////// Boss
	// SongBoss_Texture 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SongBossMOVE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/SongBoss/Move/SongBoss%d.png", TEX_NORMAL, 16)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SongBossIDLE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/SongBoss/Idle/SongBoss%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SongBossATTACK_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/SongBoss/Attack/SongBoss%d.png", TEX_NORMAL, 12)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SongBossHIT_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/SongBoss/Hit/SongBoss%d.png", TEX_NORMAL, 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SongBossDIE_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/SongBoss/Die/SongBoss%d.png", TEX_NORMAL, 10)), E_FAIL);
	// Floor
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SongBossFloor_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/SongBoss/Die/SongBoss%d.png", TEX_NORMAL, 10)), E_FAIL);


	////////////NPC
	// NPC_TalkWindow
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TalkWindow_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/TalkWindow/TalkWindow.png", TEX_NORMAL, 1)), E_FAIL);

	// NPC_Dog, Grandfather, GuitarMan, HoodMan, SmileMan, WinkMan
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NPC_Dog_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/NPC/Dog/Dog%d.png", TEX_NORMAL, 33)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NPC_Grandfather_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/NPC/Grandfather/Grandfather%d.png", TEX_NORMAL, 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NPC_GuitarMan_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/NPC/GuitarMan/GuitarMan%d.png", TEX_NORMAL, 22)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NPC_HoodMan_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/NPC/HoodMan/HoodMan%d.png", TEX_NORMAL, 21)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NPC_SmileMan_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/NPC/SmileMan/SmileMan%d.png", TEX_NORMAL, 13)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NPC_WinkMan_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/NPC/WinkMan/WinkMan%d.png", TEX_NORMAL, 17)), E_FAIL);

	/////////////UI
	// UI_HP
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_HealthBar_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/HealthBar/HealthBar0.png", TEX_NORMAL, 1)), E_FAIL);
	
	// UI_HPGauge
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_HPGauge_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/HealthBar/HealthBar1.png", TEX_NORMAL, 1)), E_FAIL);

	// UI_QuickSlot
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_QuickSlot_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/QuickSlot/QuickSlot.png", TEX_NORMAL, 1)), E_FAIL);
	
	// UI_Inventory
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_Inventory_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Inventory/Inventory.png", TEX_NORMAL, 1)), E_FAIL);
	
	// UI_EquipWindow
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_EquipWindow_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/EquipWindow/EquipWindow.png", TEX_NORMAL, 1)), E_FAIL);



	m_bFinish = true;

	//lstrcpy(m_szLoading, L"Loading Complete!!!!!");

	return 0;
}

_uint CLoading::Loading_ForBoss(void)
{
	return 0;
}

unsigned int CLoading::Thread_Main(void * pArg)
{
	CLoading*	pLoading = (CLoading*)pArg;
	_uint iFlag = 0;

	// 사실 switch문으로 분명하게 분기하고 있기 때문에 임계영역으로 굳이 지정할 필요는 없다.
	EnterCriticalSection(&(pLoading->Get_Crt()));

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		// iFlag = pLoading->Loading_ForBoss();
		break;
	}

	LeaveCriticalSection(&(pLoading->Get_Crt()));

	return iFlag;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading*		pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		MSG_BOX("Loading Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}
