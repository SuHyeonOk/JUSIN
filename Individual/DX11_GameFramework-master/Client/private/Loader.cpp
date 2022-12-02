#include "stdafx.h"
#include "..\public\Loader.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "Terrain.h"
#include "Player.h"

#include "Map_Garden.h"

#include "Food.h"
#include "Coin.h"
#include "Page.h"

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
		
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

	// Page
	/* For.Prototype_Component_Model_Enchiridion_Page_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_2/Enchiridion_Page_2.fbx"))))
		return E_FAIL;

	// Coin
	/* For.Prototype_Component_Model_CoinBronze */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinBronze"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_1_Bronze/Coin_1_Bronze.fbx"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinSilver */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinSilver"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_2_Silver/Coin_2_Silver.fbx"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinGold */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinGold"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_3_Gold/Coin_3_Gold.fbx"))))
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

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
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

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 버퍼
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 모델

	// Player
	/* For.Prototype_Component_Model_Fiona */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Fiona/Fiona.fbx"))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Finn */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Finn"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Finn/Finn.fbx"))))
	//	return E_FAIL;

	// Map
	/* For.Prototype_Component_Model_Garden */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Garden"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/map/Garden/Garden.fbx"))))
		return E_FAIL;

	// Monster


	// Food
	/* For.Prototype_Component_Model_Royal_Tart */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Royal_Tart"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Royal_Tart/Royal_Tart.fbx"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Burrito */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Burrito"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Food/Burrito/Burrito.fbx"))))
		return E_FAIL;

	// Coin
	/* For.Prototype_Component_Model_CoinBronze */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinBronze"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_1_Bronze/Coin_1_Bronze.fbx"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinSilver */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinSilver"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_2_Silver/Coin_2_Silver.fbx"))))
		return E_FAIL;
	/* For.Prototype_Component_Model_CoinGold */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CoinGold"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Coin/Coin_3_Gold/Coin_3_Gold.fbx"))))
		return E_FAIL;

	// Hp
	///* For.Prototype_Component_Model_Cyclop_Tears */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cyclop_Tears"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Hp/Cyclop_Tears/Cyclop_Tears.fbx"))))
	//	return E_FAIL;

	// Page
	/* For.Prototype_Component_Model_Enchiridion_Page_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchiridion_Page_2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Item/Page/Enchiridion_Page_2/Enchiridion_Page_2.fbx"))))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. ")); ////////////////////////////////////////////////////////////////// 셰이더
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

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
