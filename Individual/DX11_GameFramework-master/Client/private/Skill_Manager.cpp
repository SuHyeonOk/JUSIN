#include "stdafx.h"
#include "..\public\Skill_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{
}

void	CSkill_Manager::Page_Use(ITEMINDEX	iIndex)
{
	m_tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(iIndex);	// ���� ���� ���� �ε���

	if(0 < m_arrPageCount[m_tPlayerSkill.eSkill])								// ����ó�� 0���� Ŭ��, �� ��ų�� ���� �� ��� �����ϴ�.
		m_arrPageCount[m_tPlayerSkill.eSkill] -= 1;								// ������ �ϳ� ����

	CUI_Manager::GetInstance()->Set_IsIcon_Index(iIndex, false);

	cout <<  0 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE) << " | " <<
			 1 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_TWO) << " | " <<
			 2 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_THREE) << " | " <<
			 3 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_FOUR) << endl;
}

void	CSkill_Manager::Page_PickUp(CGameObject * pOther)
{
	if (L"Item_Page_Paint" == pOther->Get_Tag())
	{
		m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;		// �浹�� ��ü�� ���� ����
		SkillIcon(PLAYERSKILL::SKILL::PAINT);				// ����ִ� UI â Ȯ���ؼ� Icon �ֱ�
	}
	else if (L"Item_Page_Marcelint" == pOther->Get_Tag())
	{
		m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT] += 1;
		SkillIcon(PLAYERSKILL::SKILL::MARCELINT);
	}

	cout << "PAINT : " << m_arrPageCount[PLAYERSKILL::SKILL::PAINT] << " | " <<
		 "MARCELINT : " << m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT] << " | " <<

		endl;
}

void	CSkill_Manager::SkillIcon(PLAYERSKILL::SKILL eSkill)
{
	for (_int i = 0; i < ITEMINDEX_END; ++i)
	{
		if (false == CUI_Manager::GetInstance()->Get_IsIcon_Index(ITEMINDEX(i)))	// ���� IconIndex �� ����ִٸ�!
		{
			CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEMINDEX(i), true);		// �ش� â�� �׸���.
			CUI_Manager::GetInstance()->Set_SkillIcon(ITEMINDEX(i), eSkill);		// �� ���� ���� ���� ȹ���� �������� �־��
			break;
		}
	}
}

void CSkill_Manager::Free()
{
}