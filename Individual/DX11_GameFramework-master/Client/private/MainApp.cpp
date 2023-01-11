#include "stdafx.h"
#include "..\public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"

#include "Obj_Manager.h"
#include "DataManager.h"
#include "Effect_Manager.h"
#include "ItemManager.h"
#include "Skill_Manager.h"
#include "UI_Manager.h"
#include "Utilities_Manager.h"

#include "Model.h"
#include "Sky.h"
#include "Finn.h"
#include "Jake.h"
#include "Finn_Weapon.h"
#include "Jake_Weapon.h"
#include "S_StunChick.h"
#include "S_PaintWork.h"
#include "S_Marceline.h"
#include "S_Fiona.h"
#include "S_Skill_Weapon.h"

#include "O_TextureObject.h"

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

	//if(m_pGameInstance->Key_Down(DIK_F12))
	//	g_bNavigationRender = true;

#ifdef _DEBUG
	m_dTimeAcc += TimeDelta;	// FPS 를 위한 시간
#endif 

	// Collider _R, _L
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_ITME);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_NPC);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_OBJ);

	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_MONSTER);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_BULLET);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_PLAYER, CCollider_Manager::COL_M_WEAPON);
	CGameInstance::GetInstance()->Update_Col(CCollider_Manager::COL_P_WEAPON, CCollider_Manager::COL_BULLET);
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

#ifdef  _DEBUG
	++m_iNumDraw;

	if (m_dTimeAcc >= _double(1))
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		m_iNumDraw = 0;
		m_dTimeAcc = 0;
	}

	SetWindowText(g_hWnd, m_szFPS);
	//pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("Font_한글"), _float2(100.f, 0.f), 0.f, _float2(1.f, 1.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
#endif //  _DEBUG

	m_pGameInstance->Render_Level();

	m_pGameInstance->Present();

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

	// MakeSpriteFont "폰트이름" /FontSize:32 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 출력파일이름.spritefont
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Comic"), TEXT("../Bin/Resources/Fonts/131.spritefont"))))
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

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Point_Instancing"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 30))))
		return E_FAIL;
#pragma endregion

#pragma region Shader
	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */	// 터레인 말고는 안 씀
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
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

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint_Instance.hlsl"), VTXPOINTINSTANCE_DECLARATION::Elements, VTXPOINTINSTANCE_DECLARATION::iNumElements))))
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
	// 스턴
	/* For.Prototype_Component_Model_S_StunChick */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_StunChick"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Stun/Confused_Birds/Confused_Birds.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_GameObject_S_StunChick */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_StunChick"),
		CS_StunChick::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 총알
	/* For.Prototype_Component_Model_S_PaintWork_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Blue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Skill/PaintWork/PaintWork_Blue/PaintWork_Blue.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_PaintWork_Magenta */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Magenta"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Skill/PaintWork/PaintWork_Magenta/PaintWork_Magenta.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_S_PaintWork_Yellow */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_PaintWork_Yellow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/Skill/PaintWork/PaintWork_Yellow/PaintWork_Yellow.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_GameObject_S_PaintWork */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_PaintWork"),
		CS_PaintWork::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));	// 날 보고

	// 마르셀린
	/* For.Prototype_Component_Model_S_Marceline */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Marceline"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Marceline/Marceline.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_GameObject_S_Marceline */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Marceline"),
		CS_Marceline::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// 피오나
	/* For.Prototype_Component_Model_S_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Model_S_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Meshes/Skill/Fiona/Fiona.fbx", PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_GameObject_S_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Fiona"),
		CS_Fiona::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_S_Weapon */												// 스킬 중 콜라이더만 필요한 경우 사용하는 객체
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_S_Weapon"),
		CS_Skill_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	// 2D Object
	/* For.Prototype_Component_Texture_O_TextureObject */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_O_TextureObject"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Portal/Portal.png")))))
		return E_FAIL;
	// 2D Obj
	/* For.Prototype_GameObject_O_TextureObject */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_O_TextureObject"),
		CO_TextureObject::Create(m_pDevice, m_pContext))))
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
	_int a = 0;

#ifdef _DEBUG
	FreeConsole();
#endif

	CDataManager::GetInstance()->DestroyInstance();
	CEffect_Manager::GetInstance()->DestroyInstance();
	CItemManager::GetInstance()->DestroyInstance();
	CObj_Manager::GetInstance()->DestroyInstance();
	CSkill_Manager::GetInstance()->DestroyInstance();
	CUI_Manager::GetInstance()->DestroyInstance();
	CUtilities_Manager::GetInstance()->DestroyInstance();

	m_pGameInstance->Clear_ImguiObjects();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}
