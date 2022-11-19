#include "stdafx.h"
#include "..\public\Monster_Tool.h"

#include <fstream>
#include "../Default/ImGui/ImGuiFileDialog.h"

CMonster_Tool::CMonster_Tool()
{
}

void CMonster_Tool::Tick_Monster_Tool()
{
	ImGui::Begin("Monster Tool");

	static _int		iHp = 0;

	ImGui::SliderInt("input HP", &iHp, 0, 100);
	ImGui::Text("HP : %d", iHp);

	static char buf[32] = u8"NIHONGO";
	if (ImGui::TreeNode("UTF-8 Text"))
	{
		ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
		ImGui::TreePop();
	}


	if (ImGui::Button("Save"))
	{
		ofstream ofs("../../Data/Info.txt", ios::app);

		if (!ofs)//(ofs.fail())
		{
			MSG_BOX("Data File Error!");
			return;
		}

		ofs << iHp << endl;
		ofs.close();
		//WinExec("notepad.exe ../../Data/Info.txt", SW_SHOW);

		return;
	}

	if (ImGui::Button("Load"))
	{
		ifstream ofs("../../Data/Info.txt");

		if (!ofs)//(ofs.eof() == false)
		{
			MSG_BOX("Data File Load Error!");
			return;
		}

		ofs >> iHp;
		ofs.close();

		return;
	}

	// ff
	static _bool bCheckbox = false;
	static _float fSliderFloat = 0.f;
	static _float fColorEdit3 = 0.f;
	static _int iButtonUp = 0;

	ImGui::Checkbox("Checkbox", &bCheckbox);
	ImGui::SliderFloat("float", &fSliderFloat, 0.0f, 1.0f);
	ImGui::ColorEdit3("dd", &fColorEdit3);
	if (ImGui::Button("Button"))
		iButtonUp++;

	static wstring	sName = L"Ok";
	static _int		iLevel = 0;
	//static _int		iHp = 100;
	static _int		iAttack = 5;

	//ImGui::SameLine(); // 다음 것 이어서
	ImGui::Text("bCheckbox = %o", bCheckbox);
	ImGui::Text("fSliderFloat = %f", fSliderFloat);
	ImGui::Text("fColorEdit3 = %f", fColorEdit3);
	ImGui::Text("iButtonUp = %d", iButtonUp);

	// demo
	static _bool	show_demo_window = false;
	static _int		m_iTemp = 0;
	if (ImGui::Button("show_demo_window") && m_iTemp == 0)
	{
		show_demo_window = true;
		++m_iTemp;
	}
	else if (m_iTemp == 2)
	{
		show_demo_window = false;
		m_iTemp = 0;
	}
	ImGui::Text("m_iTemp = %d", m_iTemp);

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	else
		ImGui::ShowAboutWindow(&show_demo_window);


	ImGui::ShowDebugLogWindow(&show_demo_window); // MousePos값알 수 있던데 '-'

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End(); // 창에 필요한 기능을 다 넣었다면 End로 닫기
}

void CMonster_Tool::Free()
{
}
