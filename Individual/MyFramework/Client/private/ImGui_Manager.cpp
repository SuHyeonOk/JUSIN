#include "stdafx.h"
#include "..\public\ImGui_Manager.h"

#if defined(USE_IMGUI)
#include "Monster_Tool.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

void CImGui_Manager::Ready_Imgui()
{
	// 그래픽장치 초기화 하는 것 처럼 ImGui도 초기화 과정이 필요함
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// ImGui의 스타일 세팅
	ImGui::StyleColorsDark();

	// GameInstance 얻어오기
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	// 사용하려는 플랫폼과 렌더러(장치) 세팅
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pGameInstance->Get_Device(), m_pGameInstance->Get_Context());

	// ImGui_Manager의 멤버변수로 장치랑 윈도우 핸들 설정 (생성자에서 미리 설정하면 가끔 문제 생겨서 초기화 할 때 설정함)
	m_pDevice = m_pGameInstance->Get_Device();
	m_pDeviceContext = m_pGameInstance->Get_Context();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	// Tool 추가
	m_pMonsterTool = new CMonster_Tool();
}

void CImGui_Manager::Tick_Imgui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGui_Manager::Render_Imgui()
{
//	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGui_Manager::Monster_Tool()
{
	m_pMonsterTool->Tick_Monster_Tool();
}

void CImGui_Manager::Free()
{
	// Tool	
	Safe_Release(m_pMonsterTool);

	// 멤버변수 Release
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	// ImGui Free
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#endif