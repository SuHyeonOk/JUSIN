#include "stdafx.h"
#include "..\public\Monster_Tool.h"

#include <fstream>
#include "../Default/ImGui/ImGuiFileDialog.h"


CMonster_Tool::CMonster_Tool()
{
}

void CMonster_Tool::Tick_Monster_Tool()
{
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
}

void CMonster_Tool::Free()
{
}
