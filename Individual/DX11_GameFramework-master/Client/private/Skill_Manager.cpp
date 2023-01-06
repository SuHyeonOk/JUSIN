#include "stdafx.h"
#include "..\public\Skill_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{
}

void	CSkill_Manager::SkillIcon_Tick()
{
	if (PLAYERSKILL::PAINT == m_tPlayerSkill.eSkill)
		CUI_Manager::GetInstance()->Set_SkillIcon(0, 1);



}

void CSkill_Manager::Free()
{
}