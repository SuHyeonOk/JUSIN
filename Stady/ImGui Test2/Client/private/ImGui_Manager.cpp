#include "stdafx.h"
#include "..\public\ImGui_Manager.h"

#include <fstream>
#include "../Default/ImGui/ImGuiFileDialog.h"

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
}

void CImGui_Manager::Tick_Imgui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Info Tool");
	Info_Tool();
	ImGui::End(); // 창에 필요한 기능을 다 넣었다면 End로 닫기
}

void CImGui_Manager::Render_Imgui()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGui_Manager::Info_Tool()
{
	static _int		iHp = 0;

	ImGui::SliderInt("input HP", &iHp, 0, 100);
	ImGui::Text("HP : %d", iHp);

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	if (m_pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEYSTATE(DIM_LB)) & 0x8000)
	{
		iHp += 1;
	}

	if (ImGui::Button("DataAdd"))
	{

	}

	if (ImGui::Button("Save"))
	{
		wofstream ofs;
		ofs.open("../../Data/Info.txt", ios::out | ios::app);

		if (!ofs.fail())
		{
			ofs << iHp << endl;
			ofs.close();
		}
		else
		{
			MSG_BOX("Data File Error!");
			return;
		}


		WinExec("notepad.exe ../../Data/Info.txt", SW_SHOW);

		return;
	}


	//if (ImGui::Button("Save"))
	//{

	//	ImGuiFileDialog::Instance()->OpenDialog("Test", "Test Test", nullptr, ".", 1, nullptr, ImGuiFileDialogFlags_None);
	//}



	//static _bool bCheckbox = false;
	//static _float fSliderFloat = 0.f;
	//static _float fColorEdit3 = 0.f;
	//static _int iButtonUp = 0;

	//ImGui::Checkbox("Checkbox", &bCheckbox);
	//ImGui::SliderFloat("float", &fSliderFloat, 0.0f, 1.0f);
	//ImGui::ColorEdit3("dd", &fColorEdit3);
	//if (ImGui::Button("Button"))
	//	iButtonUp++;

	//static wstring	sName = L"Ok";
	//static _int		iLevel = 0;
	//static _int		iHp = 100;
	//static _int		iAttack = 5;

	////ImGui::SameLine(); // 다음 것 이어서
	//ImGui::Text("bCheckbox = %o", bCheckbox);
	//ImGui::Text("fSliderFloat = %f", fSliderFloat);
	//ImGui::Text("fColorEdit3 = %f", fColorEdit3);
	//ImGui::Text("iButtonUp = %d", iButtonUp);

	//// demo
	//static _bool	show_demo_window = false;
	//static _int		m_iTemp = 0;
	//if (ImGui::Button("show_demo_window") && m_iTemp == 0)
	//{
	//	show_demo_window = true;
	//	++m_iTemp;
	//}
	//else if(m_iTemp == 2)
	//{
	//	show_demo_window = false;
	//	m_iTemp = 0;
	//}
	//ImGui::Text("m_iTemp = %d", m_iTemp);

	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);
	//else
	//	ImGui::ShowAboutWindow(&show_demo_window);


	//ImGui::ShowDebugLogWindow(&show_demo_window); // MousePos값알 수 있던데 '-'

	 ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void CImGui_Manager::Free()
{
	// 멤버변수 Release
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	// ImGui Free
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}