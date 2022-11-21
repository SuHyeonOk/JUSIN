#include "stdafx.h"
#include "..\public\Tool_Monster.h"

#include <fstream>
#include "../Default/ImGui/ImGuiFileDialog.h"

CTool_Monster::CTool_Monster()
{
}

void CTool_Monster::Tick_Monster_Tool()
{
	ImGui::Begin("Monster Tool"); // 시작

	//static _float		fPos[3] = {0};
	//ImGui::InputFloat3("MonsterPos", &fPos[3]);

	static _float		fPosX = 0;
	ImGui::InputFloat("MonsterPosX", &fPosX);
	
	//static _int		iHp = 0;

	//ImGui::SliderInt("input HP", &iHp, 0, 100);
	//ImGui::Text("HP : %d", iHp);

	//static char buf[32] = u8"NIHONGO";
	//if (ImGui::TreeNode("UTF-8 Text"))
	//{
	//	ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
	//	ImGui::TreePop();
	//}

	if (ImGui::Button("Save"))
	{
		ofstream ofs;
		ofs.open("../../Data/Info.txt", ios::app);

		if (!ofs)//(ofs.fail())
		{
			MSG_BOX("Data File Save Error!");
			return;
		}

		ofs << fPosX << endl;
		//ofs << fPos[0] << "|" << fPos[1] << "|" << fPos[2] << endl;
		ofs.close();
		WinExec("notepad.exe ../../Data/Info.txt", SW_SHOW);
	}

	//list<string> pos;
	list<_float> pos;

	if (ImGui::Button("Load"))
	{
		ifstream ofs("../../Data/Info.txt");

		if (!ofs)//(ofs.eof() == false)
		{
			MSG_BOX("Data File Load Error!");
			return;
		}

		while (!ofs.eof())
		{
			string tmp;
			//getline(ofs, tmp);
			//pos.push_back(tmp);

			//_float aa;
			//fopen(ofs, aa);
			//pos.push_back(ofs);
		}

		//ofs >> fPos[0] >> "|" >> ;
		ofs.close();
	}

	//// ff
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
	////static _int		iHp = 100;
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
	//else if (m_iTemp == 2)
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

	ImGui::End(); // 창에 필요한 기능을 다 넣었다면 End로 닫기
}

void CTool_Monster::Free()
{
}
