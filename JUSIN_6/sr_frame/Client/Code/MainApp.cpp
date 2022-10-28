#include "stdafx.h"
#include "..\Header\MainApp.h"
#include "AbstFactory.h"
#include "ImGuiMgr.h"
#include "AnimationTool.h"
#include "MapTool.h"
#include "UItool.h"
#include "TerrainCubeMap.h"
#include "Stage.h"
#include "time.h"
#include "Shader.h"
#include "ArrowCubeMgr.h"
#include "PlayerController.h"
#include "StatComponent.h"
#include "PlayerController.h"
#include "GeomancerController.h"
#include "ZombieController.h"
#include "CreeperController.h"
#include "SkeletonController.h"
#include "EnchanterController.h"
#include "KoukuController.h"
#include "RedStoneCubeController.h"
#include "RedStoneMonstrosityController.h"
#include "DamageFontMgr.h"
#include "ObjectStoreMgr.h"
#include "SatonController.h"
#include "EndermanController.h"
#include "LeaperController.h"
USING(Engine)

CMainApp::CMainApp()
	: m_pGraphicDev(nullptr)
{
}


CMainApp::~CMainApp()
{
	
}

HRESULT CMainApp::Ready_MainApp(void)
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Proto(), E_FAIL);
	CAbstFactory::Ready_Factories(m_pGraphicDev);

	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	srand((unsigned)time(NULL));


	return S_OK;
}

_int CMainApp::Update_MainApp(const _float & fTimeDelta)
{
	CGameUtilMgr::s_fTimeDelta = fTimeDelta;

	Engine::SetUp_InputDev();

	NULL_CHECK_RETURN(m_pManagementClass, -1);

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	// CImGuiMgr::LoggerWindow();
	// CImGuiMgr::SceneSwitcher();

	m_pManagementClass->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp(void)
{
	NULL_CHECK(m_pManagementClass);

	m_pManagementClass->LateUpdate_Scene();


}

void CMainApp::Render_MainApp(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	Engine::Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	ImGui::EndFrame();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

	Engine::Render_End();
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	// 디바이스 초기화
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass), E_FAIL);
	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	// InputDev
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// Font
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Minecraft15", L"Minecraft", 15, 20, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Bold15", L"Kim jung chul Gothic Bold", 15, 20, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Bold20", L"Kim jung chul Gothic Bold", 20, 25, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Bold30", L"Kim jung chul Gothic Bold", 30, 35, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Bold70", L"Kim jung chul Gothic Bold", 70, 75, FW_NORMAL), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Bold40", L"Kim jung chul Gothic Regular", 40, 45, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Regular15", L"Kim jung chul Gothic Regular", 15, 20, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Regular25", L"Kim jung chul Gothic Regular", 25, 30, FW_NORMAL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Gothic_Regular30", L"Kim jung chul Gothic Regular", 30, 35, FW_NORMAL), E_FAIL);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX9_Init(m_pGraphicDev);

	// 매니저 초기화
	CArrowCubeMgr::Create(m_pGraphicDev);
	CDamageFontMgr::GetInstance()->SetGraphicDev(m_pGraphicDev);
	CObjectStoreMgr::GetInstance();

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement)
{

	// scene 생성시 Management에 자동 set됩니다. scene을 보존하고 싶으면 ref 1올려서 저장하기
	// Scene::Ready_Scene 에서 Management에 set
	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagement), E_FAIL);
	(*ppManagement)->AddRef();

	  // CSceneFactory::LoadScene("Loading1", "Stage_Default", true ,0);
	CSceneFactory::LoadScene("Loading1", "Logo", true ,500);
	// CSceneFactory::LoadScene("Loading1", "NetStage_Start", true ,0);
	// CSceneFactory::LoadScene("Loading1", "Animation Tool", true, 0);
	// CSceneFactory::LoadScene("Loading1", "Stage_Start", true, 0);
	



	return S_OK;
}

HRESULT CMainApp::Ready_Proto()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexCom", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Terrain0.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Terrain_WaterCom", CTerrainShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Water_Terrain_Test1.fx", VTXCNTX, VTXCNTZ, VTXITV
		, _vec2(0.f, 0.f), _vec2(1.f, 0.f), _vec2(1.f, 1.f), _vec2(0.f, 1.f), 36,36)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LazerBeamCom", CTerrainShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Terrain_Water_Twssaest1.fx", 2, 2, 1
		, _vec2(0.f, 0.f), _vec2(1.f, 0.f), _vec2(1.f, 1.f), _vec2(0.f, 1.f), 1, 1)), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GradationCom", CTerrainShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Lazer.fx", 2, 2, 1
		, _vec2(0.f, 0.f), _vec2(1.f, 0.f), _vec2(1.f, 1.f), _vec2(0.f, 1.f), 1, 1)), E_FAIL);

	

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Terrain_Water_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Water4.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTexCom", Engine::CCubeTex::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/Sky_Box_%d.dds", TEX_CUBE, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MinecraftCubeTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/MinscraftCubeTile/CubeTile_%d.dds", TEX_CUBE, 194)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformCom", Engine::CTransform::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BossCubeTile", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/BossCubeTile/boss_%d.dds", TEX_CUBE, 17)), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WeaponTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/weapon/weapon_%d.png", TEX_NORMAL, 5)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Electric", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/T_ElectricArcs.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Cloud", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/T_Smoke_deformedBall_Small.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Circle", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/T_fullcircle_explosion_124_spritesheet.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Spit", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/CircleTest.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Crack", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Crack1.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LazerLightTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/T_GodRayLight.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LazerBeamTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/BeaconBeam_0.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LoadingBoxTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/loader.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GradationTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/drop_beam.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LazerCircleTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Circle.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Stun", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Stun.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Fascinated", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Fascinate.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Heal", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Heal_Circle1.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Attack_Circle", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/CircleTest.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Heal_Particle", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Heart_Particle.png", TEX_NORMAL)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Lava_Particle", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/JJH/Lava_Particlel.png", TEX_NORMAL)), E_FAIL);

	// FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcShaderCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation.fx", _vec2(0.0f, 0.0f), _vec2(0.25f, 0.0f), _vec2(0.25f, 0.25f), _vec2(0.0f, 0.25f))), E_FAIL);


	//Shader
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeShaderCom", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_Player.hlsl")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColumnShaderCom", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_Column.hlsl")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CreeperShaderCom", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_Creeper.hlsl")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DynamiteShaderCom", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_Dynamite.hlsl")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EndermanTrailShaderCom", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_EndermanTrail.hlsl")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcShaderCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation.fx",_vec2(0.0f,0.0f),_vec2(0.25f,0.f),_vec2(0.25f,0.25f),_vec2(0.0f,0.25f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShockPowderCloudCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/ShockPowder_Cloud.fx",_vec2(0.0f, 0.0f), _vec2(0.25f, 0.f), _vec2(0.25f, 0.25f), _vec2(0.0f, 0.25f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CreeperExCloudCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/CreeperEx_Cloud.fx", _vec2(0.0f, 0.0f), _vec2(0.25f, 0.f), _vec2(0.25f, 0.25f), _vec2(0.0f, 0.25f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WalkCloudCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Walk_Cloud.fx", _vec2(0.0f, 0.0f), _vec2(0.25f, 0.f), _vec2(0.25f, 0.25f), _vec2(0.0f, 0.25f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RollCloudCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Roll_Cloud.fx", _vec2(0.0f, 0.0f), _vec2(0.25f, 0.f), _vec2(0.25f, 0.25f), _vec2(0.0f, 0.25f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChainLightningCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_ChainLightning.fx",_vec2(0.0f,0.0f),_vec2(0.25f,0.f),_vec2(0.25f,0.25f),_vec2(0.0f,0.25f))), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SphereCom_Ring", CSphereMesh::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Half_Shpere.fx", L"../Bin/Resource/Texture/JJH/Test.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SphereCom_Lava", CSphereMesh::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Half_Shpere.fx", L"../Bin/Resource/Texture/JJH/Test.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SphereCom_L", CSphereMesh::Create(m_pGraphicDev, L"../Bin/Resource/Shader/TextureMapping.fx", L"../Bin/Resource/Texture/JJH/YellowSphere.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SphereCom_M", CSphereMesh::Create(m_pGraphicDev, L"../Bin/Resource/Shader/TextureMapping.fx", L"../Bin/Resource/Texture/JJH/YellowSphere.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StunCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FaciCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Faci.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HealCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Crack.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BlueCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_BlueCircle.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Ready_Range_CircleCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Attack_Range_Circle_0.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Attack_Range_CircleCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Attack_Range_Circle_1.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LazerCircleCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_LazerCircle.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HeartParticleCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Crack.fx", _vec2(0.0f, 0.0f), _vec2(0.25f, 0.f), _vec2(0.25f, 1.0f), _vec2(0.0f, 1.f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LoadingBoxCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_LoadingBox.fx", _vec2(0.0f, 0.0f), _vec2(0.11718f, 0.0f), _vec2(0.11718f, 0.11718f), _vec2(0.0f, 0.11718f))), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Lava_ParticleCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Crack.fx", _vec2(0.0f, 0.0f), _vec2(0.25f, 0.0f), _vec2(0.25f, 0.25f), _vec2(0.0f, 0.25f))), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CrackCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Crack.fx", _vec2(0.0f, 0.0f), _vec2(0.5f, 0.0f), _vec2(0.5f, 0.5f), _vec2(0.0f, 0.5f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Crack_DecalCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Decal.fx", _vec2(0.0f, 0.0f), _vec2(0.5f, 0.0f), _vec2(0.5f, 0.5f), _vec2(0.0f, 0.5f))), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Lazer_Com", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Lazer.fx", _vec2(0.0f, 0.0f), _vec2(0.5f, 0.0f), _vec2(0.5f, 0.5f), _vec2(0.0f, 0.5f))), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShaderCircleCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/UVAnimation_Circle.fx",_vec2(0.0f, 0.0f), _vec2(0.25f, 0.0f), _vec2(0.25f, 0.5f), _vec2(0.0f, 0.5f))), E_FAIL);
	
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SpitCom", CRcShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Golem_Spit.fx", _vec2(0.0f, 0.0f), _vec2(1.f, 0.0f), _vec2(1.f, 1.f), _vec2(0.0f, 1.f))), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_VoxelTex_Sword", CVoxelTex::Create(m_pGraphicDev, "../Bin/Resource/Texture/weapon/weapon_0.png", 0.08f)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_VoxelTex_Glaive", CVoxelTex::Create(m_pGraphicDev, "../Bin/Resource/Texture/weapon/weapon_1.png", 0.08f)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_VoxelTex_Crossbow", CVoxelTex::Create(m_pGraphicDev, "../Bin/Resource/Texture/weapon/weapon_2.png", 0.08f)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_VoxelTex_Axe", CVoxelTex::Create(m_pGraphicDev, "../Bin/Resource/Texture/weapon/weapon_3.png", 0.08f)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_VoxelTex_Bow", CVoxelTex::Create(m_pGraphicDev, "../Bin/Resource/Texture/weapon/weapon_4.png", 0.08f)), E_FAIL);

	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainCubeTexCom",CTerrainCubeTex::Create(m_pGraphicDev, L"../Bin/Resource/Map/MapTest.dat", 1)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorCom", CCalculator::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CollisionCom", CCollisionCom::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StatCom", CStatComponent::Create()), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlantTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Plant/plant_%d.png", TEX_NORMAL, 3)), E_FAIL);


	// controller
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerController", CPlayerController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GeomancerController", CGeomancerController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ZombieController", CZombieController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CreeperController", CCreeperController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkeletonController", CSkeletonController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EnchanterController", CEnchanterController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RedStoneCubeController", CRedStoneCubeController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RedStoneMonstrosityController", CRedStoneMonstrosityController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_KoukuController", CKoukuController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SatonController", CSatonController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EndermanController", CEndermanController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperController", CLeaperController::Create()), E_FAIL);

	// remote controller
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRemoteController", CPlayerRemoteController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ZombieRemoteController", CZombieRemoteController::Create()), E_FAIL);
	// FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_KoukuController", CKoukuController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SatonRemoteController", CSatonRemoteController::Create()), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_KoukuRemoteController", CKoukuRemoteController::Create()), E_FAIL);


	// UI
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexCom", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_%d.png", TEX_NORMAL, 42)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_InventoryUI_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/InventoryUI/UI_%d.png", TEX_NORMAL, 21)), E_FAIL);

	// FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Loading", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Loading/loading%d.png", TEX_NORMAL, 1)), E_FAIL);

	//items
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Emerald", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SingleTex/Emerald.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Apple", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SingleTex/Apple.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Bread", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SingleTex/Bread.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DenseBrew", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SingleTex/DenseBrew.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArrowBundle", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SingleTex/ArrowBundle.png", TEX_NORMAL)), E_FAIL);


	// Object
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BirdWhite", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BirdBrown", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BirdsWhiteTextCom", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Object/BirdsWhite/BirdsWhite_%d.png", TEX_NORMAL, 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BirdsBrownTextCom", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Object/BirdsBrown/BirdsBrown_%d.png", TEX_NORMAL, 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Totem", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SingleTex/Totem.png", TEX_NORMAL)), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkySphere", CShereBuffer::Create(m_pGraphicDev, 20, 20)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkySphereShaderCom", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_Sky.hlsl")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkySphereTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkySphere/sky_%d.png", TEX_NORMAL, 4)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GhostTrail", CShader::Create(m_pGraphicDev, L"../Bin/Resource/Shader/Shader_GhostTrail.hlsl")), E_FAIL);

	return S_OK;
}

CMainApp * CMainApp::Create(void)
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("MainApp Create Failed");
		Safe_Release<CMainApp*>(pInstance);		
	}
	
	return pInstance;
}

void CMainApp::Free(void)
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	CClientServiceMgr::DestroyInstance();
	CObjectStoreMgr::DestroyInstance();
	CArrowCubeMgr::DeleteInst();
	CDamageFontMgr::DestroyInstance();
	CAbstFactory::ReleaseGraphicDev();
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);
	Safe_Release(m_pManagementClass);


	Engine::Release_Utility();
	Engine::Release_System();
}
