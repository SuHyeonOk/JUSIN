#include "stdafx.h"
#include "..\public\Loader.h"

#include "GameInstance.h"

#include "Terrain.h"
#include "Player.h"

// Map
#include "Map_Garden.h"
#include "Map_Skeleton.h"
#include "Map_Skeleton_Boss.h"
#include "Map_MiniGame.h"

// UI
#include "UI_.h"
#include "UI_3DTexture.h"
#include "UI_Talk.h"
#include "UI_Monster.h"
#include "UI_Monster_Bar.h"
#include "UI_Monster_BarBack.h"
#include "UI_Informacion.h"
#include "UI_Player_Bar.h"
#include "UI_Player_BarBack.h"
#include "UI_Level_Bar.h"
#include "UI_Level_BarBack.h"
#include "UI_Level_Number.h"
#include "UI_Weapon.h"
#include "UI_Weapons.h"
#include "UI_Scroll.h"
#include "UI_SkillIcon.h"
#include "UI_Scroll_Count.h"
#include "UI_MiniMap.h"
#include "UI_Snail.h"
#include "UI_Boss.h"
#include "UI_Boss_Bar.h"
#include "UI_Boss_BarBack.h"
#include "UI_Inventory_X.h"
#include "UI_Inventory.h"
#include "UI_Icon.h"
#include "UI_CurrentIcon.h"
#include "UI_ShopBase.h"
#include "UI_ShopIndex.h"
#include "UI_ShopSword.h"
#include "UI_CutScene.h"

// Effect
#include "Effect_Rect_Instancing.h"
#include "Effect_Point_Instancing.h"
#include "E_DieCenter.h"
#include "E_NoLook_Grow.h"
#include "E_Burst.h"
#include "E_Beneficial.h"
#include "E_Food_Up.h"
#include "E_Alpha_Change.h"
#include "E_Look_Grow.h"
#include "E_Alpha_Rotation.h"
#include "E_Look_Up.h"
#include "E_Look_Alpha.h"
#include "Player_Talk.h"
#include "E_FlyingEnvironment.h"
#include "E_NoLook_Alpha.h"
#include "E_Pigs_Rainbow.h"

// Obj
#include "O_Box.h"
#include "O_BearTrap.h"
#include "O_Portal.h"
#include "O_Collider.h"
#include "FamilySword.h"
#include "PennyCopy.h"
#include "JakeCopy.h"
#include "O_Cloud.h"

// NPC
#include "N_Bubblegum.h"
#include "N_KeyMan.h"
#include "N_Doctor.h"
#include "N_GooseShop.h"
#include "N_BMO.h"

// Skill
#include "S_Change_Magic.h"
#include "S_Jake_Son.h"
#include "S_Jake_Son_Transform.h"
#include "S_Jake_Son_Twister.h"
#include "S_FinnAndJake.h"

// Weapon
#include "W_PigWarrior.h"
#include "W_GhostFist.h"
#include "W_Skeleton_Shield.h"

// Item
#include "Food.h"
#include "Coin.h"
#include "Page.h"
#include "Key.h"
#include "Heart.h"

// Bullet
#include "B_2DBullet.h"
#include "B_3DBullet.h"
#include "B_AnimBullet.h"

// Monster
#include "M_PigWarrior.h"
#include "M_Pigs.h"
#include "M_Gronmes.h"
#include "M_Tree_Witch.h"
#include "M_Magic_Man.h"
#include "M_Mimic.h"
#include "M_Ghost.h"
#include "M_Skeleton_Shield.h"
#include "M_Skeleton_Archer.h"
#include "M_Penny.h"

// Boss
#include "M_Gary_Boss.h"
#include "Boss_S_Scream.h"
#include "Boss_S_Cage.h"
#include "Boss_Fan.h"
#include "Boss_S_Wind.h"

// MiniGame
#include "MiniGmae_Collider.h"
#include "Knives_Rain.h"
#include "Lady_Rainicorn.h"
#include "Snail.h"
#include "Cake.h"
#include "Squirrel.h"
#include "Korean_Food.h"

#include "Obj_Manager.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingThread(void* pArg)
{
	CObj_Manager::GetInstance()->Set_NextLevel(true);
	CObj_Manager::GetInstance()->Set_Current_Level(LEVEL_LOADING);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlay();
		break;
	case LEVEL_MINIGAME:
		pLoader->Loading_ForMiniGame();
		break;
	case LEVEL_SKELETON:
		pLoader->Loading_ForSkeleton();
		break;
	case LEVEL_SKELETON_BOSS:
		pLoader->Loading_ForSkeleton_Boss();
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

/* 로고를 위한 원형을 생성한다. */
HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	///* For.Prototype_Component_Texture_Logo */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/AT_title.dds")))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	///* For.Prototype_GameObject_BackGround */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
	//	CBackGround::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

/* 게임플레이를 위한 원형을 생성한다. */
HRESULT CLoader::Loading_ForGamePlay()
{
	if (2 == CObj_Manager::GetInstance()->Get_Loading_Count())
	{
		lstrcpy(m_szLoadingText, TEXT("미니게임을 종료합니다. "));

		m_isFinished = true;
		return S_OK;
	}

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region 텍스쳐
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 텍스처
	// SktBox
	/* For.Prototype_Component_Texture_Sky_Garden */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_Garden"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Garden.dds")))))
		return E_FAIL;

	// 2D Bullet
	/* For.Prototype_Component_Texture_B_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Star/Star.png")))))
		return E_FAIL;

	UI_Texture();
	Effect_Texture();

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
	/* For.Prototype_Component_Model_Apple_Pie */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Apple_Pie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Apple_Pie/Apple_Pie.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Chewed_Food */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chewed_Food"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Chewed_Food/Chewed_Food.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Soy_People */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Soy_People"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Soy_People/Soy_People.fbx", PivotMatrix))))
		return E_FAIL;
	
	// Cyclop
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cyclop_Tears"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Cyclop_Tears/Cyclop_Tears.fbx", PivotMatrix))))
		return E_FAIL;
	// Key
	/* For.Prototype_Component_Model_Key */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Key"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Key/Key.fbx", PivotMatrix))))
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
	// Page
	/* For.Prototype_Component_Model_Enchiridion_Page_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_2/Enchiridion_Page_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_3/Enchiridion_Page_3.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_4 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_4/Enchiridion_Page_4.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_5 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_5/Enchiridion_Page_5.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_6 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_6/Enchiridion_Page_6.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_7 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_7/Enchiridion_Page_7.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_8 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_8/Enchiridion_Page_8.fbx", PivotMatrix))))
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

	// 3D Bullet
	/* For.Prototype_Component_Model_B_FireDragon_Area_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_FireDragon_Area_FX"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/FireDragon_Area_FX/FireDragon_Area_FX.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_B_Tree_Witch_Roots_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Tree_Witch_Roots_FX"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Tree_Witch_Roots_FX/Tree_Witch_Roots_FX.fbx", PivotMatrix))))
		return E_FAIL;

	// Jake_Son
	/* For.Prototype_Component_Model_S_Jake_son_A */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_A"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_A/Jake_son_A.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_Jake_son_B */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_B"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_B/Jake_son_B.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_Jake_son_C */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_C"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_C/Jake_son_C.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_Jake_son_D */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_D"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_D/Jake_son_D.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_Jake_son_E */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_E"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_E/Jake_son_E.fbx", PivotMatrix))))
		return E_FAIL;

	///////////////////////////////////////////////////////////// ANIM

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// Skell
	/* For.Prototype_Component_Model_S_Magic_Man_Jake */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Magic_Man_Jake"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Player/Magic_Man_Jake/Magic_Man_Jake.fbx", PivotMatrix))))
		return E_FAIL;

	// NPC
	/* For.Prototype_Component_Model_N_Princess_Bubblegum */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_Princess_Bubblegum"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/Princess Bubblegum/Princess Bubblegum.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_N_KeyPerGarden */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_KeyPerGarden"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/Key Per Garden/Key Per Garden.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_N_COMMERCE_TEST */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_COMMERCE_TEST"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/COMMERCE_TEST/COMMERCE_TEST.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_N_BMO*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_BMO"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/BMO/BMO.fbx", PivotMatrix))))
		return E_FAIL;

	// Obj
	/* For.Prototype_Component_Model_O_Box*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_O_Box"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Box/Box.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_O_BearTrap*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_O_BearTrap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/BearTrap/BearTrap.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_O_Portal_Off*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_O_Portal_Off"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Object/Portal_Off/Portal_Off.fbx", PivotMatrix))))
		return E_FAIL;

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
	/* For.Prototype_Component_Model_M_Mimic */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_M_Mimic"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Mimic_Monster/Mimic_Monster.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_JakeCopy */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JakeCopy"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Player/JakeCopy/JakeCopy.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cloud_0*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cloud_0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Cloud/Cloud_0.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cloud_1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cloud_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Cloud/Cloud_1.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cloud_2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cloud_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Cloud/Cloud_2.fbx", PivotMatrix))))
		return E_FAIL;
	
#pragma endregion

#pragma region 셰이더
	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 셰이더

#pragma endregion

#pragma region 네비게이션
	lstrcpy(m_szLoadingText, TEXT("네비게이션정보생성중"));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/Navi_Garden.txt")))))
		return E_FAIL;
#pragma endregion

#pragma region 객체
	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. ")); ////////////////////////////////////////////////////////////////// 객체 원형
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	UI_Create();
	Effect_Create();

	// Obj
	/* For.Prototype_GameObject_O_Box */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_Box"),
		CO_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_O_BearTrap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_BearTrap"),
		CO_BearTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_O_Portal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_PortalOff"),
		CO_Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// NPC
	/* For.Prototype_GameObject_N_Bubblegum */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_Bubblegum"),
		CN_Bubblegum::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_N_KeyMan */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_KeyMan"),
		CN_KeyMan::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_N_GooseShop */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_GooseShop"),
		CN_GooseShop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_N_BMO */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_BMO"),
		CN_BMO::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Map
	/* For.Prototype_GameObject_Map_Garden */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Garden"),
		CMap_Garden::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Skill
	/* For.Prototype_GameObject_Magic_Man_Jake */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Change_Magic"),
		CS_Change_Magic::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_S_Jake_Son */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Jake_Son"),
		CS_Jake_Son::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Item
	/* For.Prototype_GameObject_Food */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Food"),
		CFood::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hp"),
		CFood::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Key */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Key"),
		CKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Heart */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Heart"),
		CHeart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Coin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
		CCoin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
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
	/* For.Prototype_GameObject_B_AnimBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_AnimBullet"),
		CB_AnimBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Monster Weapon
	/* For.Prototype_GameObject_PigWarrior_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PigWarrior_Weapon"),
		CW_PigWarrior::Create(m_pDevice, m_pContext))))
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
	/* For.Prototype_GameObject_M_Mimic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Mimic"),
		CM_Mimic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// JakeCopy
	/* For.Prototype_GameObject_JakeCopy */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_JakeCopy"),
		CJakeCopy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Collider
	/* For.Prototype_GameObject_O_Collider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_Collider"),
		CO_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Cloud
	/* For.Prototype_GameObject_O_Cloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_Cloud"),
		CO_Cloud::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForMiniGame()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	_matrix			PivotMatrix = XMMatrixIdentity();
	
	// Map
	/* For.Prototype_Component_Model_MiniGame */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_MiniGame"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/map/MiniGame/MiniGame.fbx", PivotMatrix))))
		return E_FAIL;

	// Game
	/* For.Prototype_Component_Model_Knives_Rain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Knives_Rain"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/MiniGame/Knives_Rain/Knives_Rain.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Lady_Rainicorn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Lady_Rainicorn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/MiniGame/Lady_Rainicorn/Lady_Rainicorn.fbx", PivotMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_Cake_Blue */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Cake_Blue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/MiniGame/Cake/Cake_Blue.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cake_Magenta */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Cake_Magenta"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/MiniGame/Cake/Cake_Magenta.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cake_Yellow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Cake_Yellow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/MiniGame/Cake/Cake_Yellow.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Korean_Food */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Korean_Food"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/MiniGame/Korean_Food/Korean_Food.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// 플레이어
	/* For.Prototype_Component_Model_S_FinnAndJake */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_FinnAndJake"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/FinnAndJake/FinnAndJake.fbx", PivotMatrix))))
		return E_FAIL;

	// 달팽이
	/* For.Prototype_Component_Model_Snail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Snail"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/MiniGame/Snail/Snail.fbx", PivotMatrix))))
		return E_FAIL;

	// 다람쥐
	/* For.Prototype_Component_Model_Squirrel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Squirrel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/MiniGame/Squirrel/Squirrel.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));

	// Map
	/* For.Prototype_GameObject_Map_MiniGame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_MiniGame"),
		CMap_MiniGame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Player
	/* For.Prototype_GameObject_FinnAndJake */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FinnAndJake"),
		CS_FinnAndJake::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Collider
	/* For.Prototype_GameObject_MiniGame_Collider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGame_Collider"),
		CMiniGame_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 미니 게임 종류
	/* For.Prototype_GameObject_Knives_Rain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Knives_Rain"),
		CKnives_Rain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Lady_Rainicorn */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lady_Rainicorn"),
		CLady_Rainicorn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Ckae */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ckae"),
		CCake::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Squirrel */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Squirrel"),
		CSquirrel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Korean_Food */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Korean_Food"),
		CKorean_Food::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 달팽이
	/* For.Prototype_GameObject_Snail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snail"),
		CSnail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#ifdef F2_SKELETON
	SkeletonTemp();
#endif

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForSkeleton()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#ifdef F2_SKELETON
	SkeletonTemp();
#endif

#pragma region 텍스쳐
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 텍스처
	
	// SktBox
	 /* For.Prototype_Component_Texture_Sky_Garden_Night */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Texture_Sky_Garden_Night"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Garden_Night.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region 버퍼
	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 버퍼

#pragma endregion

#pragma region 모델
	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 모델

	_matrix			PivotMatrix = XMMatrixIdentity();

	// Map
	/* For.Prototype_Component_Model_Skeleton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/map/Skeleton/Skeleton.fbx", PivotMatrix))))
		return E_FAIL;

	///////////////////////////////////////////////////////////// ANIM

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	// Monster
	/* For.Prototype_Component_Model_M_Ghost_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Ghost_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Ghost/Ghost_1/Ghost_1.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Ghost_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Ghost_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Ghost/Ghost_2/Ghost_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Ghost_3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Ghost_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Ghost/Ghost_3/Ghost_3.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Archer_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Archer_2/Skeleton_Archer_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Archer_3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Archer_3/Skeleton_Archer_3.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Archer_4 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Archer_4/Skeleton_Archer_4.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Shield_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Shield_2/Skeleton_Shield_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Shield_4 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Shield_4/Skeleton_Shield_4.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Penny */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Penny"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Penny/Penny.fbx", PivotMatrix))))
		return E_FAIL;


	// Weapon
	/* For.Prototype_Component_Model_W_skeleton_sword */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_W_skeleton_sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/skeleton_sword/skeleton_sword.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_skeleton_shield */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_W_skeleton_shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/skeleton_shield/skeleton_shield.fbx", PivotMatrix))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_Component_Model_B_Bone */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_B_Bone"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Bone/Bone.fbx", PivotMatrix))))
		return E_FAIL;

	// Npc
	/* For.Prototype_Component_Model_N_Doctor_Princess */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_N_Doctor_Princess"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/Doctor Princess/Doctor Princess.fbx", PivotMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region 객체
	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. ")); ////////////////////////////////////////////////////////////////// 객체 원형
	 // Map
	 /* For.Prototype_GameObject_Map_Skeleton */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Skeleton"),
		CMap_Skeleton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
													  
	// Monster
	/* For.Prototype_GameObject_M_Ghost */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Ghost"),
		CM_Ghost::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Skeleton_Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Skeleton_Shield"),
		CM_Skeleton_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Skeleton_Archer */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Skeleton_Archer"),
		CM_Skeleton_Archer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Penny */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Penny"),
		CM_Penny::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PennyCopy */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PennyCopy"),
		CPennyCopy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FamilySword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FamilySword"),
		CFamilySword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// MonsterWeapon
	/* For.Prototype_GameObject_W_GhostFist */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_W_GhostFist"),
		CW_GhostFist::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_W_Skeleton_Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_W_Skeleton_Shield"),
		CW_Skeleton_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Npc
	/* For.Prototype_GameObject_N_Doctor */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_Doctor"),
		CN_Doctor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForSkeleton_Boss()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#ifdef F2_SKELETON
	SkeletonTemp();
	SkeletonBossTemp();
#endif

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	// SktBox
	/* For.Prototype_Component_Texture_Sky_Dead */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Texture_Sky_Dead"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Dead.dds")))))
		return E_FAIL;

	// Boss_Skill
	/* For.Prototype_Component_Texture_Scream */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Texture_Scream"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Scream/Scream_template.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Wind */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Texture_Wind"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Wind/Finisher_360_Wave_1.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

	_matrix			PivotMatrix = XMMatrixIdentity();

	// Map
	/* For.Prototype_Component_Model_Skeleton_Boss */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_Skeleton_Boss"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/map/Skeleton_Boss/Skeleton_Boss.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// Boss
	/* For.Prototype_Component_Model_Gary */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_Gary"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/Gary/Gary.fbx", PivotMatrix))))
		return E_FAIL;
	// Boss Skill
	/* For.Prototype_Component_Model_B_Boss_Fite_Rock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_B_Boss_Fite_Rock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Fire_Rock/Fire_Rock.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_B_Boss_Death_Cage */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_B_Boss_Death_Cage"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Bullet/Death_Cage/Death_Cage.fbx", PivotMatrix))))
		return E_FAIL;
	// BossFan
	/* For.Prototype_Component_Model_M_Skeleton_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_M_Skeleton_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_1/Skeleton_1.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_S_JakeSonsTransform */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_S_JakeSonsTransform"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/JakeSonsTransform/JakeSonsTransform.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_JakeSonsTwister */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON_BOSS, TEXT("Prototype_Component_Model_S_JakeSonsTwister"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/JakeSonsTwister/JakeSonsTwister.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	// Map
	/* For.Prototype_GameObject_Map_Skeleton */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Skeleton_Boss"),
		CMap_Skleton_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Boss
	/* For.Prototype_GameObject_M_Gary_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Gary_Boss"),
		CM_Gary_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Boss_S_Scream */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_S_Scream"),
		CBoss_S_Scream::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Boss_S_Cage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_S_Cage"),
		CBoss_S_Cage::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Boss_Fan */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Fan"),
		CBoss_Fan::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Boss_S_Wind */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_S_Wind"),
		CBoss_S_Wind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Skill
	/* For.Prototype_GameObject_S_JakeSonTransform */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_JakeSonTransform"),
		CS_Jake_Son_Transform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_S_Jake_Son_Twister */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Jake_Son_Twister"),
		CS_Jake_Son_Twister::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::UI_Texture()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// 3D UI
	/* For.Prototype_Component_Texture_UI_FindEnemy_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_FindEnemy_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FindEnemy_FX/FindEnemy_FX.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_LooseEnemy_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LooseEnemy_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LooseEnemy_FX/LooseEnemy_FX.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Surprised */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Surprised"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FindEnemy_FX/Finn and Jake are surprised.png")))))
		return E_FAIL;

	// Npc
	/* For.Prototype_Component_Texture_UI_Talk */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Talk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Talk/Talk.png")))))
		return E_FAIL;

	// Monster
	/* For.Prototype_Component_Texture_UI_Monster */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Monster"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI_Moonster/UI_Monster_%d.dds"), 15))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Monster_HealthBar_Bar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Monster_HealthBar_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI_Moonster/Enemies_HealthBar_Bar.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Monster_HealthBar_BarBack */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Monster_HealthBar_BarBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI_Moonster/Enemies_HealthBar_BarBack.png")))))
		return E_FAIL;

	// Boss
	/* For.Prototype_Component_Texture_UI_Boss */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI_Moonster/barra_boss_base.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Boss_HealthBar_Bar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss_HealthBar_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI_Moonster/barra_boss_Death.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Boss_HealthBar_BarBack */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss_HealthBar_BarBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI_Moonster/barra_boss_White.png")))))
		return E_FAIL;

	// Player 
	/* For.Prototype_Component_Texture_UI_Finn_Informacion */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Finn_Informacion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/Finn_informacion.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Jake_Informacion */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Jake_Informacion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/Jake_informacion.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_HPBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/barra_verde.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_HPBarBack */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HPBarBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/barra_verde_White.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_LEVELBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LEVELBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/barra_amarilla.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_LEVELBar */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LEVELBarBack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/barra_amarilla_white.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Level_Number */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Level_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevlNumber/lvl%d.png"), 16))))
		return E_FAIL;

	// Weapon
	/* For.Prototype_Component_Texture_UI_Weapons */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Weapons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/weapons_%d.png"), 2))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Weapon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/Weapon_%d.png"), 12))))
		return E_FAIL;

	// Skill
	/* For.Prototype_Component_Texture_UI_Skill_Scroll */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Scroll"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/Scroll.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Skill_Icon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/Skill_%d.png"), 8))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Skill_Scroll_Count */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Skill_Scroll_Count"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/Scroll_Count.png")))))
		return E_FAIL;

	// mini map
	/* For.Prototype_Component_Texture_UI_Mini_Map */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mini_Map"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mini_Map/Mini_Map_%d.png"), 3))))
		return E_FAIL;

	// MiniGmae
	/* For.Prototype_Component_Texture_UI_MiniGame_Snail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_MiniGame_Snail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniGame/Snail.png")))))
		return E_FAIL;

	// Inventory
	/* For.Prototype_Component_Texture_UI_Inventory_X */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Inventory_X"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/enchiridion_pc_Key1.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Inventario */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Inventario"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/inventario.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Icon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Icon_%d.png"), 7))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_CurrentIcon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CurrentIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/inventario selecionador.png")))))
		return E_FAIL;

	// Shop
	/* For.Prototype_Component_Texture_UI_Shop */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Shop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Shop_%d.png"), 5))))
		return E_FAIL;

	// CutScene
	/* For.Prototype_Component_Texture_UI_CutScene */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CutScene"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/CutScene/CutScene.png")))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::UI_Create()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_UI_3DTexture */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_3DTexture"),
		CUI_3DTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Talk */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Talk"),
		CUI_Talk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Monster
	/* For.Prototype_GameObject_UI_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster"),
		CUI_Monstser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Monster_HealthBar_Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster_HealthBar_Bar"),
		CUI_Monstser_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Monster_HealthBar_BarBack */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster_HealthBar_BarBack"),
		CUI_Monstser_BarBack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Boss
	/* For.Prototype_GameObject_UI_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Boss"),
		CUI_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Boss_HealthBar_Bar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Boss_HealthBar_Bar"),
		CUI_Boss_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Boss_HealthBar_BarBack */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Boss_HealthBar_BarBack"),
		CUI_Boss_BarBack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Player
	/* For.Prototype_GameObject_UI_Informacion */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_Informacion"),
		CUI_Informacion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_HPBar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_HPBar"),
		CUI_Player_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_HPBarBack */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Player_HPBarBack"),
		CUI_Player_BarBack::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Level_HPBar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Level_HPBar"),
		CUI_Level_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Level_HPBarBack */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Level_HPBarBack"),
		CUI_Level_BarBack::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Level_Number */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Level_Number"),
		CUI_Level_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Weapon
	/* For.Prototype_GameObject_Player_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon"),
		CUI_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Weapons */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapons"),
		CUI_Weapons::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Skill
	/* For.Prototype_GameObject_Player_Skill_Scroll */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Skill_Scroll"),
		CUI_Scroll::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Skill_Icon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Skill_Icon"),
		CUI_SkillIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Skill_Scroll_Count */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Skill_Scroll_Count"),
		CUI_Scroll_Count::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// MiniMap
	/* For.Prototype_GameObject_UI_MninMap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MninMap"),
		CUI_MninMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// MiniGame
	/* For.Prototype_GameObject_UI_Snail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Snail"),
		CUI_Snail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Inventory
	/* For.Prototype_GameObject_UI_Inventory */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Inventory_X"),
		CUI_Inventory_X::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Inventory */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Inventory"),
		CUI_Inventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Icon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Icon"),
		CUI_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_CurrentIcon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_CurrentIcon"),
		CUI_CurrentIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Shop
	/* For.Prototype_GameObject_UI_ShopBase */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ShopBase"),
		CUI_ShopBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_ShopIndex */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ShopIndex"),
		CUI_ShopIndex::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_ShopSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ShopSword"),
		CUI_ShopSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_CutScene */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_CutScene"),
		CUI_CutScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/* For.Prototype_GameObject_UI */ // 최상위 부모
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
		CUI_::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Effect_Texture()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Monset Die
	/* For.Prototype_Component_Texture_E_DieCenter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_DieCenter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Monster_Die/DieCenter_%d.png"), 9))))
		return E_FAIL;

	// Skill Marceline
	/* For.Prototype_Component_Texture_E_Skill_Marceline_Sound */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Skill_Marceline_Sound"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Marceline/Marceline_Sound_Note.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Skill_Marceline_Waves */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Skill_Marceline_Waves"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Marceline/Marceline_Waves.png")))))
		return E_FAIL;

	// Skill Cange Smoke
	/* For.Prototype_Component_Texture_E_Change_Smoke */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Change_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Smoke/Gnomes_Hit_FX_2_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Change_ColorSmoke */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Change_ColorSmoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Smoke/Gnomes_Hit_FX_1_D.png")))))
		return E_FAIL;

	// Food
	/* For.Prototype_Component_Texture_E_Food_Change */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Food_Change"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Food/Effect_Food_%d.png"), 8))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Food_Up */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Food_Up"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Food/HealFX_2_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Food_Hp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Food_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Food/SpellKnight05Particle01.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Food_MinusHp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Food_MinusHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Food/SpellKnight05Particle02.png")))))
		return E_FAIL;

	// Hit
	/* For.Prototype_Component_Texture_E_Hit_Cahange */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Hit_Cahange"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Hit_Cahnge_Effect_%d.png"), 4))))
		return E_FAIL;

	// Swim
	/* For.Prototype_Component_Texture_E_Swim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Swim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Swim/Water_Wave_FX_1.png")))))
		return E_FAIL;

	// Paint
	/* For.Prototype_Component_Texture_E_Paint_Firecracker */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Paint_Firecracker"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Paint/Water_Wave_FX_3.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Paint_Circle */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Paint_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Paint/CharacterShadow_2.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Star_Random */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Star_Random"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Paint/Star_Random.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Paint_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Paint_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Paint/Paint_Star.png")))))
		return E_FAIL;

	// Star
	/* For.Prototype_Component_Texture_E_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Star/Star.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Star3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Star3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Star/Star3.png")))))
		return E_FAIL;

	// Shield
	/* For.Prototype_Component_Texture_E_Jake_Shield */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Jake_Shield"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Jake_Shield.png")))))
		return E_FAIL;

	// Coin
	/* For.Prototype_Component_Texture_E_Heart */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Heart"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Coin/Pigs_Explosion_Cuore_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Flower */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Flower"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Coin/Pigs_Explosion_Flower_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Leaf */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Leaf"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Coin/TreeWitch_Explosion_leaves_D.png")))))
		return E_FAIL;

	// JakeSon
	/* For.Prototype_Component_Texture_E_Jake_Son */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Jake_Son"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Jake_Son/Jake_Son_%d.png"), 4))))
		return E_FAIL;

	// Boss
	/* For.Prototype_Component_Texture_E_Boss_Boom_Fire */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Boom_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Fire_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Fire */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/FX_Fire_1_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Fire_Wave */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Fire_Wave"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Fire_Wave_1_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Burn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Burn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Burn_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Skeleton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Skeleton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Death_Scream_Skull_FX_1.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Skeleton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_SkeletonBig"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Death_Scream.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Smoke */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/FX_Fire_Smoke_3_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Potal_0 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Potal_0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Dead_Teleport_FX_1_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Potal_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Potal_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Portal_FX_2_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Boss_Potal_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Boss_Potal_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Artifact_FX_1.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_CutScene_Fire */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_CutScene_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/CutScene_Fire_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_CutScene_SmallFire */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_CutScene_SmallFire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/FX_Fire_1.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_E_Potal_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Potal_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Golden_Wings_Stars_D.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_E_Potal_Star_Blue */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_E_Potal_Star_Blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Golden_Wings_Stars_Blue.png")))))
		return E_FAIL;
	
	// PlayerTalk
	/* For.Prototype_Component_Texture_PlayerTalk_Jake */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerTalk_Jake"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerTalk/Jake_%d.png"), 7))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_PlayerTalk_Finn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerTalk_Finn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/PlayerTalk/Finn_%d.png"), 7))))
		return E_FAIL;

	// Environment
	/* For.Prototype_Component_Texture_Butterflies_Bule */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_Bule"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_Bule_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Butterflies_Red */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_Red"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_Red_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Butterflies_Yellow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_Yellow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_Yellow_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Butterflies_BuleL */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_BuleL"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_BuleL_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Butterflies_RedL */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_RedL"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_RedL_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Butterflies_YellowL */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_YellowL"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_YellowL_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Butterflies_Luminous */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Butterflies_Luminous"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Butterflies_Luminous_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_FireSparks */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireSparks"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/FireSparks_%d.png"), 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_FireSparks_Small */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireSparks_Small"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Environment/Fire_1_Sparks_D.png")))))
		return E_FAIL;

	// TreeMonster
	/* For.Prototype_Component_Texture_Tree_Witch */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Witch"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Tree_Witch/tree_witch_pinches_2_D.png")))))
		return E_FAIL;

	// MiniGame
	/* For.Prototype_Component_Texture_MiniGame_Heart */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniGame_Heart"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/MiniGame/Heart.png")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Squirrel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Squirrel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/MiniGame/Squirrel_Cooldown_FX_1.png")))))
		return E_FAIL;

	// Pigs_Rainbow
	/* For.Prototype_Component_Texture_Pigs_Rainbow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pigs_Rainbow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Pigs_Rainbow/Pigs_Rainbow.png")))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Effect_Create()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_E_DieCenter */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_DieCenter"),
		CE_DieCenter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_E_NoLook_Alpha*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_NoLook_Alpha"),
		CE_NoLook_Alpha::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_E_NoLook_Grow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_NoLook_Grow"),
		CE_NoLook_Grow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_E_Look_Grow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Look_Grow"),
		CE_Look_Grow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_E_Burst*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Burst"),
		CE_Burst::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_E_Beneficial*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Beneficial"),
		CE_Beneficial::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_E_Food_Up*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Food_Up"),
		CE_Food_Up::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_E_Alpha_Change*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Alpha_Change"),
		CE_Alpha_Change::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_E_Alpha_Rotation*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Alpha_Rotation"),
		CE_Alpha_Rotation::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_E_Look_Up*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Look_Up"),
		CE_Look_Up::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_E_Look_Alpha*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Look_Alpha"),
		CE_Look_Alpha::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// PlayerTalk
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Talk"),
		CPlayer_Talk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Environment
	/* For.Prototype_GameObject_E_FlyingEnvironment*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_FlyingEnvironment"),
		CE_FlyingEnvironment::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Pigs_Rainbow
	/* For.Prototype_GameObject_E_Pigs_Rainbow*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Pigs_Rainbow"),
		CE_Pigs_Rainbow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::SkeletonTemp()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// SktBox
	/* For.Prototype_Component_Texture_Sky_Garden */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_Garden"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Garden.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/Navi_Skeleton_Boss.txt")))))
		return E_FAIL;

	// 2D Bullet
	/* For.Prototype_Component_Texture_B_Star */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_B_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/2DBullet/Star/Star.png")))))
		return E_FAIL;

	UI_Texture();
	Effect_Texture();

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix			PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_O_Box*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_O_Box"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Box/Box.fbx", PivotMatrix))))
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
	/* For.Prototype_Component_Model_Apple_Pie */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Apple_Pie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Apple_Pie/Apple_Pie.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Chewed_Food */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chewed_Food"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Chewed_Food/Chewed_Food.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Soy_People */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Soy_People"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Soy_People/Soy_People.fbx", PivotMatrix))))
		return E_FAIL;


	// Cyclop
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cyclop_Tears"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Cyclop_Tears/Cyclop_Tears.fbx", PivotMatrix))))
		return E_FAIL;
	// Key
	/* For.Prototype_Component_Model_Key */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Key"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Key/Key.fbx", PivotMatrix))))
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

	// Page
	/* For.Prototype_Component_Model_Enchiridion_Page_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_2/Enchiridion_Page_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_3/Enchiridion_Page_3.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_4 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_4/Enchiridion_Page_4.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_5 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_5/Enchiridion_Page_5.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_6 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_6/Enchiridion_Page_6.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_7 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_7/Enchiridion_Page_7.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enchiridion_Page_8 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_8/Enchiridion_Page_8.fbx", PivotMatrix))))
		return E_FAIL;

	// 3D Bullet
	/* For.Prototype_Component_Model_B_Tree_Witch_Roots_FX */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_B_Tree_Witch_Roots_FX"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Tree_Witch_Roots_FX/Tree_Witch_Roots_FX.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_S_Jake_son_D */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_D"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_D/Jake_son_D.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_Jake_son_E */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_S_Jake_son_E"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Jake_son/Jake_son_E/Jake_son_E.fbx", PivotMatrix))))
		return E_FAIL;
 
	// NPC
	/* For.Prototype_Component_Model_N_COMMERCE_TEST */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_N_COMMERCE_TEST"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/NPC/COMMERCE_TEST/COMMERCE_TEST.fbx", PivotMatrix))))
		return E_FAIL;

	// Obj
	/* For.Prototype_Component_Model_O_BearTrap*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_O_BearTrap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/BearTrap/BearTrap.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_O_Portal_Off*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_O_Portal_Off"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Object/Portal_Off/Portal_Off.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cloud_0*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cloud_0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Cloud/Cloud_0.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cloud_1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cloud_1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Cloud/Cloud_1.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Cloud_2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cloud_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Object/Cloud/Cloud_2.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Obj
	/* For.Prototype_GameObject_O_Box */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_Box"),
		CO_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_O_BearTrap */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_BearTrap"),
		CO_BearTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_O_Portal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_PortalOff"),
		CO_Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// NPC
	/* For.Prototype_GameObject_N_Bubblegum */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_Bubblegum"),
		CN_Bubblegum::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_N_GooseShop */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_N_GooseShop"),
		CN_GooseShop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Item
	/* For.Prototype_GameObject_Food */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Food"),
		CFood::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hp"),
		CFood::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Key */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Key"),
		CKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Heart */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Heart"),
		CHeart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Coin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Coin"),
		CCoin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Page */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Page"),
		CPage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_GameObject_B_AnimBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_AnimBullet"),
		CB_AnimBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_B_RandomBullet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_RandomBullet"),
		CB_3DBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_S_Jake_Son */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Jake_Son"),
		CS_Jake_Son::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Cloud
	/* For.Prototype_GameObject_O_Cloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_Cloud"),
		CO_Cloud::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	UI_Create();
	Effect_Create();

	if (FAILED(pGameInstance->Clone_GameObject(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::SkeletonBossTemp()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

	_matrix			PivotMatrix = XMMatrixIdentity();

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// 보스 맵 때만!
	/* For.Prototype_Component_Model_M_Skeleton_Archer_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Archer_2/Skeleton_Archer_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Archer_3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Archer_3/Skeleton_Archer_3.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Archer_4 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Archer_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Archer_4/Skeleton_Archer_4.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Shield_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Shield_2/Skeleton_Shield_2.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_M_Skeleton_Shield_4 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_M_Skeleton_Shield_4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Monster/All_Skeleton/Skeleton_Shield_4/Skeleton_Shield_4.fbx", PivotMatrix))))
		return E_FAIL;

	// Weapon
	/* For.Prototype_Component_Model_W_skeleton_sword */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_W_skeleton_sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/skeleton_sword/skeleton_sword.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_skeleton_shield */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_W_skeleton_shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/skeleton_shield/skeleton_shield.fbx", PivotMatrix))))
		return E_FAIL;

	// Bullet
	/* For.Prototype_Component_Model_B_Bone */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKELETON, TEXT("Prototype_Component_Model_B_Bone"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Bullet/Random_Bullet/Bone/Bone.fbx", PivotMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	/* For.Prototype_GameObject_M_Skeleton_Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Skeleton_Shield"),
		CM_Skeleton_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_M_Skeleton_Archer */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_M_Skeleton_Archer"),
		CM_Skeleton_Archer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// MonsterWeapo
	/* For.Prototype_GameObject_W_Skeleton_Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_W_Skeleton_Shield"),
		CW_Skeleton_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Collider
	/* For.Prototype_GameObject_O_Collider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_Collider"),
		CO_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

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
