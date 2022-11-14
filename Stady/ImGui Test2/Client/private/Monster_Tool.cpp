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
}

void CMonster_Tool::Free()
{
}
