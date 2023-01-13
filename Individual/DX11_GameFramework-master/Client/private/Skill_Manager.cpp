#include "stdafx.h"
#include "..\public\Skill_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{
}

void	CSkill_Manager::Set_ChangeSkill_Create(_bool bIsCreate)
{
	if (true == bIsCreate)
		m_bChangeSkill_Create = true;
	else
	{
		if (CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END == m_tPlayerSkill.eSkill)
			m_bChangeSkill_Create = false;
	}
}

void	CSkill_Manager::Page_Use(ITEMINDEX	iIndex)
{
	m_tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(iIndex);	// ���� ���� ���� �ε���
	CSkill_Manager::GetInstance()->Set_Player_Skill(m_tPlayerSkill.eSkill);

	if(0 < m_arrPageCount[m_tPlayerSkill.eSkill])								// ����ó�� 0���� Ŭ��, �� ��ų�� ���� �� ��� �����ϴ�.
		m_arrPageCount[m_tPlayerSkill.eSkill] -= 1;								// ������ �ϳ� ����

	if(0 >= m_arrPageCount[m_tPlayerSkill.eSkill])								// ��ų�� 0�� �����̸� ��ųâ ���� �����Ѵ�.
		CUI_Manager::GetInstance()->Set_IsIcon_Index(iIndex, false);

	cout <<  "ĭ�� � ��ų�� �ִ��� : " << 0 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE) << " | " <<
			 1 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_TWO) << " | " <<
			 2 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_THREE) << " | " <<
			 3 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_FOUR) << endl;
}

void	CSkill_Manager::Page_PickUp(PLAYERSKILL::SKILL iIndex)
{
	if (PLAYERSKILL::SKILL::PAINT == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;		// �浹�� ��ü�� ���� ����

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::PAINT])		// ������ ������ 1�� �� ���� ��ųâ�� �߰��� �� �ִ�.
			SkillIcon(PLAYERSKILL::SKILL::PAINT);				// ����ִ� UI â Ȯ���ؼ� Icon �ֱ�
	}
	else if (PLAYERSKILL::SKILL::MARCELINT == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT])
			SkillIcon(PLAYERSKILL::SKILL::MARCELINT);
	}
	else if (PLAYERSKILL::SKILL::COIN == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::COIN] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::COIN])
			SkillIcon(PLAYERSKILL::SKILL::COIN);
	}
	else if (PLAYERSKILL::SKILL::FIONA == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::FIONA] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::FIONA])
			SkillIcon(PLAYERSKILL::SKILL::FIONA);
	}

	//if (L"Item_Page_Paint" == pOther->Get_Tag())
	//{
	//	m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;		// �浹�� ��ü�� ���� ����

	//	if(1 == m_arrPageCount[PLAYERSKILL::SKILL::PAINT])		// ������ ������ 1�� �� ���� ��ųâ�� �߰��� �� �ִ�.
	//		SkillIcon(PLAYERSKILL::SKILL::PAINT);				// ����ִ� UI â Ȯ���ؼ� Icon �ֱ�
	//}
	//else if (L"Item_Page_Marcelint" == pOther->Get_Tag())
	//{
	//	m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT] += 1;

	//	if (1 == m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT])
	//		SkillIcon(PLAYERSKILL::SKILL::MARCELINT);
	//}
	//else if (L"Item_Page_Coin" == pOther->Get_Tag())
	//{
	//	m_arrPageCount[PLAYERSKILL::SKILL::COIN] += 1;

	//	if (1 == m_arrPageCount[PLAYERSKILL::SKILL::COIN])
	//		SkillIcon(PLAYERSKILL::SKILL::COIN);
	//}
	//else if (L"Item_Page_Fiona" == pOther->Get_Tag())
	//{
	//	m_arrPageCount[PLAYERSKILL::SKILL::FIONA] += 1;

	//	if (1 == m_arrPageCount[PLAYERSKILL::SKILL::FIONA])
	//		SkillIcon(PLAYERSKILL::SKILL::FIONA);
	//}

	//cout << "��ų ���� : " << 
	//	"PAINT : " << m_arrPageCount[PLAYERSKILL::SKILL::PAINT] << " | " <<
	//	"MARCELINT : " << m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT] << " | " <<
	//	"COIN : " << m_arrPageCount[PLAYERSKILL::SKILL::COIN] << " | " <<
	//	"FIONA : " << m_arrPageCount[PLAYERSKILL::SKILL::FIONA] << " | " <<
	//	endl;
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