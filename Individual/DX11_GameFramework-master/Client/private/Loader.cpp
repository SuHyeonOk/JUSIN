#include "stdafx.h"
#include "..\public\Loader.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "Terrain.h"
#include "Player.h"

// Map
#include "Map_Garden.h"

// 3DUI
#include "UI_3DTexture.h"

// Weapon
#include "W_PigWarrior.h"

// Item
#include "Food.h"
#include "Coin.h"
#include "Page.h"

// Bullet
#include "B_2DBullet.h"
#include "B_3DBullet.h"

// Stun
#include "S_StunChick.h"

// Monster
#include "M_PigWarrior.h"
#include "M_Pigs.h"
#include "M_Gronmes.h"
#include "M_Tree_Witch.h"
#include "M_Magic_Man.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingThread(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_TOOL:
		pLoader->Loading_Tool(); // 꼭 주석!
		break;
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlay();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* 로딩을 하기위한 추가적인 흐름을 만든다 (Thread).*/
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Tool()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	// 2D Bullet
	/* For.Prototype_Component_Texture_B_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Star/Star.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_B_Circle */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Circle/Circle.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_B_Magic */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Magic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Magic/Magic.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_FindEnemy_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_FindEnemy_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FindEnemy_FX/FindEnemy_FX.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

	_matrix			PivotMatrix = XMMatrixIdentity();

	// Player
	/* For.Prototype_Component_Model_S_StunChick */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_StunChick"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Stun/Confused_Birds/Confused_Birds.fbx", PivotMatrix))))
		return E_FAIL;

	// Page
	/* For.Prototype_Component_Model_Enchiridion_Page_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_2/Enchiridion_Page_2.fbx", PivotMatrix))))
		return E_FAIL;

	// Coin
	/* For.Prototype_Component_Model_CoinBronze */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinBronze"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_1_Bronze/Coin_1_Bronze.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinSilver */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinSilver"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_2_Silver/Coin_2_Silver.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinGold */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinGold"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_3_Gold/Coin_3_Gold.fbx", PivotMatrix))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_Component_Model_B_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Star"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Bullet/Star/Star.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_B_Tree_Witch_Roots_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Tree_Witch_Roots_FX"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Tree_Witch_Roots_FX/Tree_Witch_Roots_FX.fbx", PivotMatrix))))
		return E_FAIL;

	// Weapon
	/* For.Prototype_Component_Model_W_Wooden_Sword */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_W_Wooden_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Wooden_Sword/Wooden_Sword.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_W_Cylinder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Cylinder/Cylinder.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	//// Monster 180
	/* For.Prototype_Component_Model_M_PigWarrior_BEE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_BEE"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/PigWarrior_BEE/PigWarrior_BEE.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_PigWarrior_WORKER */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_WORKER"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/PigWarrior_WORKER/PigWarrior_WORKER.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Pigs_COWBOY */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Pigs_COWBOY"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/Pigs_COWBOY/Pigs_COWBOY.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Pigs_SR */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Pigs_SR"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/Pigs_SR/Pigs_SR.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Gnomes_BLUE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gnomes_BLUE"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Gnomes/Gnomes_BLUE/Gnomes_BLUE.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Gonmes_RED */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gonmes_RED"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Gnomes/Gnomes_RED/Gnomes_RED.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Gnomes_YELLOW */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gnomes_YELLOW"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Gnomes/Gnomes_YELLOW/Gnomes_YELLOW.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Tree_Witch */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Tree_Witch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Tree_Witch/Tree_Witch.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Magic_Man */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Magic_Man"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Magic_Man/Magic_Man.fbx", PivotMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/Navigation.txt")))))
		return E_FAIL;
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Weapon
	/* For.Prototype_GameObject_PigWarrior_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PigWarrior_Weapon"),
		CW_PigWarrior::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Page
	/* For.Prototype_GameObject_Page */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Page"),
		CPage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Coin
	/* For.Prototype_GameObject_Coin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
		CCoin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Skill
	/* For.Prototype_GameObject_S_StunChick */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_StunChick"),
		CS_StunChick::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_GameObject_B_ToodyBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_ToodyBullet"),
		CB_2DBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_B_RandomBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_RandomBullet"),
		CB_3DBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 3DUI
	/* For.Prototype_GameObject_UI_3DTexture */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_3DTexture"),
		CUI_3DTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Monster
	/* For.Prototype_GameObject_M_PigWarrior */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_PigWarrior"),
		CM_PigWarrior::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Pigs */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Pigs"),
		CM_Pigs::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Gronmes */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Gronmes"),
		CM_Gronmes::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Tree_Witch */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Tree_Witch"),
		CM_Tree_Witch::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Magic_Man */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Magic_Man"),
		CM_Magic_Man::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

/* 로고를 위한 원형을 생성한다. */
HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/AT_title.dds")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

/* 게임플레이를 위한 원형을 생성한다. */
HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region 텍스쳐
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 텍스처
	///* For.Prototype_Component_Texture_Terrain */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Brush*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Filter */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
	//	return E_FAIL;

	// 2D Bullet
	/* For.Prototype_Component_Texture_B_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Star/Star.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_B_Circle */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Circle/Circle.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_B_Magic */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Magic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Magic/Magic.png")))))
		return E_FAIL;

	// 2D UI
	/* For.Prototype_Component_Texture_UI_FindEnemy_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_FindEnemy_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FindEnemy_FX/FindEnemy_FX.png")))))
		return E_FAIL;
#pragma endregion

#pragma region 버퍼
	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 버퍼
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;
#pragma endregion

#pragma region 모델
	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 모델

	_matrix			PivotMatrix = XMMatrixIdentity();

	// Map
	/* For.Prototype_Component_Model_Garden */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Garden"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/map/Garden/Garden.fbx", PivotMatrix))))
		return E_FAIL;

	// Food
	/* For.Prototype_Component_Model_Royal_Tart */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Royal_Tart"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Royal_Tart/Royal_Tart.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Burrito */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Burrito"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Burrito/Burrito.fbx", PivotMatrix))))
		return E_FAIL;

	// Coin
	/* For.Prototype_Component_Model_CoinBronze */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinBronze"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_1_Bronze/Coin_1_Bronze.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinSilver */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinSilver"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_2_Silver/Coin_2_Silver.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinGold */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinGold"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_3_Gold/Coin_3_Gold.fbx", PivotMatrix))))
		return E_FAIL;

	// Hp
	///* For.Prototype_Component_Model_Cyclop_Tears */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cyclop_Tears"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Hp/Cyclop_Tears/Cyclop_Tears.fbx"))))
	//	return E_FAIL;

	// Page
	/* For.Prototype_Component_Model_Enchiridion_Page_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_2/Enchiridion_Page_2.fbx", PivotMatrix))))
		return E_FAIL;

	// Weapon
	/* For.Prototype_Component_Model_W_Wooden_Sword */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_W_Wooden_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Wooden_Sword/Wooden_Sword.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_W_Cylinder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Cylinder/Cylinder.fbx", PivotMatrix))))
		return E_FAIL;

	///////////////////////////////////////////////////////////// ANIM

	// NPC
	/* For.Prototype_Component_Model_N_Princess_Bubblegum */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_Princess_Bubblegum"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/Princess Bubblegum/Princess Bubblegum.fbx", PivotMatrix))))
		return E_FAIL;

	// Skill
	/* For.Prototype_Component_Model_S_StunChick */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_StunChick"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Stun/Confused_Birds/Confused_Birds.fbx", PivotMatrix))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_Component_Model_B_Tree_Witch_Roots_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Tree_Witch_Roots_FX"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Tree_Witch_Roots_FX/Tree_Witch_Roots_FX.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// Player
	/* For.Prototype_Component_Model_Fiona */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Fiona/Fiona.fbx", PivotMatrix))))
		return E_FAIL;

	// Monster 
	/* For.Prototype_Component_Model_M_PigWarrior_BEE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_BEE"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/PigWarrior_BEE/PigWarrior_BEE.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_PigWarrior_WORKER */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_PigWarrior_WORKER"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/PigWarrior_WORKER/PigWarrior_WORKER.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Pigs_COWBOY */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Pigs_COWBOY"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/Pigs_COWBOY/Pigs_COWBOY.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Pigs_SR */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Pigs_SR"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Pig/Pigs_SR/Pigs_SR.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Gnomes_BLUE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gnomes_BLUE"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Gnomes/Gnomes_BLUE/Gnomes_BLUE.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Gonmes_RED */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gonmes_RED"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Gnomes/Gnomes_RED/Gnomes_RED.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Gnomes_YELLOW */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Gnomes_YELLOW"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Gnomes/Gnomes_YELLOW/Gnomes_YELLOW.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Tree_Witch */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Tree_Witch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Tree_Witch/Tree_Witch.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Magic_Man */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Magic_Man"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Magic_Man/Magic_Man.fbx", PivotMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region 셰이더
	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 셰이더
													 /* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel_Map*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel_Map"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Map.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region 네비게이션
	lstrcpy(m_szLoadingText, TEXT("네비게이션정보생성중"));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/Navigation.txt")))))
		return E_FAIL;
#pragma endregion

#pragma region 객체
	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. ")); ////////////////////////////////////////////////////////////////// 객체 원형
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Map
	/* For.Prototype_GameObject_Map_Garden */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Garden"),
		CMap_Garden::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Food
	/* For.Prototype_GameObject_Food */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Food"),
		CFood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Coin
	/* For.Prototype_GameObject_Coin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
		CCoin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Page
	/* For.Prototype_GameObject_Page */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Page"),
		CPage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_GameObject_B_ToodyBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_ToodyBullet"),
		CB_2DBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_B_RandomBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_RandomBullet"),
		CB_3DBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Monster Weapon
	/* For.Prototype_GameObject_PigWarrior_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PigWarrior_Weapon"),
		CW_PigWarrior::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 3DUI
	/* For.Prototype_GameObject_UI_3DTexture */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_3DTexture"),
		CUI_3DTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Skill
	/* For.Prototype_GameObject_S_StunChick */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_StunChick"),
		CS_StunChick::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Monster
	/* For.Prototype_GameObject_M_PigWarrior */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_PigWarrior"),
		CM_PigWarrior::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Pigs */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Pigs"),
		CM_Pigs::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Gronmes */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Gronmes"),
		CM_Gronmes::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Tree_Witch */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Tree_Witch"),
		CM_Tree_Witch::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Magic_Man */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Magic_Man"),
		CM_Magic_Man::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
