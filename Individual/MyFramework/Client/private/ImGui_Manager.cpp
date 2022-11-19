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
	// �׷�����ġ �ʱ�ȭ �ϴ� �� ó�� ImGui�� �ʱ�ȭ ������ �ʿ���
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

	// ImGui�� ��Ÿ�� ����
	ImGui::StyleColorsDark();

	// GameInstance ������
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	// ����Ϸ��� �÷����� ������(��ġ) ����
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pGameInstance->Get_Device(), m_pGameInstance->Get_Context());

	// ImGui_Manager�� ��������� ��ġ�� ������ �ڵ� ���� (�����ڿ��� �̸� �����ϸ� ���� ���� ���ܼ� �ʱ�ȭ �� �� ������)
	m_pDevice = m_pGameInstance->Get_Device();
	m_pDeviceContext = m_pGameInstance->Get_Context();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	// Tool �߰�
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

	// ������� Release
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	// ImGui Free
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

#endif