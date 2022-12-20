#include "stdafx.h"
#include "..\public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"

#include "Obj_Manager.h"
#include "DataManager.h"

#include "Model.h"
#include "Sky.h"
#include "Finn.h"
#include "Jake.h"
#include "Finn_Weapon.h"
#include "Jake_Weapon.h"
#include "Finn_Change.h"
#include "S_Change_Magic.h"

#include "UI_Informacion.h"
#include "UI_HPGauge.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 게임엔진 초기화 */
	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHIC_DESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWindowMode = GRAPHIC_DESC::WINMODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL_LOGO)))
		return E_FAIL;

#ifdef _DEBUG // 디버그 창
	if (::AllocConsole() == TRUE) {
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}
#endif

	CObj_Manager::GetInstance()->Initialized();

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(TimeDelta);

	CObj_Manager::GetInstance()->Tick(TimeDelta);

	m_dTimeAcc += TimeDelta;	// FPS 를 위한 시간

	// Collider _R, _L
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_COIN);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_FOOD);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_PAGE);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_MONSTER);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_BULLET);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_M_WEAPON);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_P_WEAPON, CCollider_Manager::COL_MONSTER);

	CGameInstance::GetInstance()->Reset_Col();		// 위에서 전체적인 충돌처리가 끝나면! 전체 콜라이더 컴포넌트를 소유한 객체들을 리스트 에서 해제, 레퍼런스 카운트를 감소 시킨다.
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pGameInstance->Render_ImGui();

	m_pGameInstance->Clear_Graphic_Device(&_float4(0.5f, 0.5f, 0.5f, 1.f));

	m_pRenderer->Draw_RenderGroup();

	m_pGameInstance->Render_Update_ImGui();

	m_pGameInstance->Render_Level();

	m_pGameInstance->Present();

#ifdef  _DEBUG

	++m_iNumDraw;

	if (m_dTimeAcc >= _double(1))
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		m_iNumDraw = 0;
		m_dTimeAcc = 0;
	}

	SetWindowText(g_hWnd, m_szFPS);

#endif //  _DEBUG

	return S_OK;
}

HRESULT CMainApp::Resize_BackBuffer()
{
	GRAPHIC_DESC	tGraphicDesc;
	ZeroMemory(&tGraphicDesc, sizeof(GRAPHIC_DESC));

	tGraphicDesc.hWnd = g_hWnd;
	if (!g_bFullScreen)
	{
		tGraphicDesc.iViewportSizeX = g_iWinSizeX;
		tGraphicDesc.iViewportSizeY = g_iWinSizeY;
	}
	else
	{
		tGraphicDesc.iViewportSizeX = GetSystemMetrics(SM_CXSCREEN);
		tGraphicDesc.iViewportSizeY = GetSystemMetrics(SM_CYSCREEN);
	}

	if (FAILED(m_pGameInstance->Update_SwapChain(tGraphicDesc.hWnd, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, g_bFullScreen, g_bNeedResizeSwapChain)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID ||
		nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

#pragma region	Buffer
	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
		CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 100))))
		return E_FAIL;
#pragma endregion

#pragma region Shader
	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel*/
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRect_Instancing.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region Collider
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
#pragma endregion

#pragma region SkyBox
	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Model_Weapon
	_matrix			PivotMatrix = XMMatrixIdentity();

	// PlayerWeapon
	/* For.Prototype_Component_Model_W_Root_sword */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_W_Root_sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Root_sword/Root_sword.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_Family_sword */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_W_Family_sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Family_sword/Family_sword.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_Golden_Sword_New */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_W_Golden_Sword_New"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Golden_Sword_New/Golden_Sword_New.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_W_Jake_Punch_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_W_Jake_Punch_Shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Weapon/Jake_Punch_Shield/Jake_Punch_Shield.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_GameObject_Finn_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Finn_Weapon"),
		CFinn_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Jake_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Jake_Weapon"),
		CJake_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL; 
#pragma endregion

#pragma region Model_Player
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));	/////////////////////////////////////////////// 180

	// Player
	/* For.Prototype_Component_Model_Finn */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_Finn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Player/Finn/Finn.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_Jake */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_Jake"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Player/Jake/Jake.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_GameObject_Finn */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Finn"),
		CFinn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Jake */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Jake"),
		CJake::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Skill
	/* For.Prototype_Component_Model_S_Magic_Man_Finn */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Magic_Man_Finn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Player/Magic_Man_Finn/Magic_Man_Finn.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_Magic_Man_Jake */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Magic_Man_Jake"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Player/Magic_Man_Jake/Magic_Man_Jake.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_GameObject_Magic_Man_Jake */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Change_Magic"),
		CS_Change_Magic::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region	UI
	/* For.Prototype_Component_Texture_UI_Finn_Informacion */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_UI_Finn_Informacion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/Finn_informacion.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_UI_Jake_Informacion */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_UI_Jake_Informacion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/Jake_informacion.dds")))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Informacion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Informacion"),
		CUI_Informacion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HPGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_UI_HPGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/barra_verde.png")))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_HPGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HPGauge"),
		CUI_HPGauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_UI_LEVELGauge */
	//if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_UI_LEVELGauge"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Informacion/barra_amarilla.png")))))
	//	return E_FAIL;
	///* For.Prototype_GameObject_UI_Informacion */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Informacion"),
	//	CUI_Informacion::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
#pragma endregion

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
#ifdef _DEBUG
	FreeConsole();
#endif

	m_pGameInstance->Clear_ImguiObjects();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}
