#include "stdafx.h"
#include "..\public\Skill_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{
}

void	CSkill_Manager::Set_ChangeSkill_Create(_bool bIsCreate)					// �÷��̾ ���� ��������
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
	if(false == CUI_Manager::GetInstance()->Get_IsIcon_Index(iIndex))			// ���� �ϴ� ��ų�� ����ִٸ� ������� �� �Ѵ�.
		return;
	
	m_tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(iIndex);	// ���� ���� ���� �ε���
	CSkill_Manager::GetInstance()->Set_Player_Skill(m_tPlayerSkill.eSkill);

	if(0 < m_arrPageCount[m_tPlayerSkill.eSkill])								// ����ó�� 0���� Ŭ��, �� ��ų�� ���� �� ��� �����ϴ�.
		m_arrPageCount[m_tPlayerSkill.eSkill] -= 1;								// ������ �ϳ� ����

	if (0 >= m_arrPageCount[m_tPlayerSkill.eSkill])								// ��ų�� 0�� �����̸�...
	{
		// ��ų â
		CUI_Manager::GetInstance()->Set_IsIcon_Index(iIndex, false);

		// �κ��丮
		Delete_InventoryIcon(m_tPlayerSkill.eSkill);
	}

	cout <<  "ĭ�� � ��ų�� �ִ��� : " << 0 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE) << " | " <<
			 1 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_TWO) << " | " <<
			 2 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_THREE) << " | " <<
			 3 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_FOUR) << endl;
}

void	CSkill_Manager::Page_PickUp(PLAYERSKILL::SKILL iIndex)
{
	if (PLAYERSKILL::SKILL::PAINT == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;			// �浹�� ��ü�� ���� ����

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::PAINT])		// ������ ������ 1�� �� ���� ��ųâ�� �߰��� �� �ִ�.
		{
			SkillIcon(PLAYERSKILL::SKILL::PAINT);				// ����ִ� UI â Ȯ���ؼ� Icon �ֱ�
			InventoryIcon(PLAYERSKILL::SKILL::PAINT);			// ����ִ� �κ�â Ȯ���ؼ� Icon �ֱ�
		}
	}
	else if (PLAYERSKILL::SKILL::MARCELINT == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::MARCELINT])
		{
			SkillIcon(PLAYERSKILL::SKILL::MARCELINT);
			InventoryIcon(PLAYERSKILL::SKILL::MARCELINT);
		}
	}
	else if (PLAYERSKILL::SKILL::COIN == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::COIN] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::COIN])
		{
			SkillIcon(PLAYERSKILL::SKILL::COIN);
			InventoryIcon(PLAYERSKILL::SKILL::COIN);
		}
	}
	else if (PLAYERSKILL::SKILL::FIONA == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::FIONA] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::FIONA])
		{
			SkillIcon(PLAYERSKILL::SKILL::FIONA);
			InventoryIcon(PLAYERSKILL::SKILL::FIONA);
		}
	}
	else if (PLAYERSKILL::SKILL::JAKESON == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::JAKESON] += 1;

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::JAKESON])
			SkillIcon(PLAYERSKILL::SKILL::JAKESON);
		// JakeSon �� ��쿡�� �κ��丮�� �̹� �߰� �Ǿ� �ֱ� ������ �Լ��� ȣ������ �ʴ´�.
	}

	//if (L"Item_Page_Paint" == pOther->Get_Tag())
	//{
	//	m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;			// �浹�� ��ü�� ���� ����

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

void		CSkill_Manager::Set_Page_Penny(PLAYERSKILL::SKILL iIndex)
{
	--m_arrPageCount[iIndex];
	Delete_InventoryIcon(iIndex);
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

void	CSkill_Manager::InventoryIcon(PLAYERSKILL::SKILL eSkill)
{
	for (_int i = _int(INVENTORYICON::ICON_TWO); i < _int(INVENTORYICON::ICON_END); ++i)
	{
		if (false == CUI_Manager::GetInstance()->Get_IsInventoryIcon_Index(INVENTORYICON(i)))
		{
			CUI_Manager::GetInstance()->Set_IsInventoryIcon_Index(INVENTORYICON(i), true);		// �ش� â�� �׸���.
			CUI_Manager::GetInstance()->Set_InventoryIcon(INVENTORYICON(i), eSkill);			// ȹ���� �������� �ִ´�.
			break;
		}
	}
}

void	CSkill_Manager::Delete_InventoryIcon(PLAYERSKILL::SKILL eCurrentSkill)
{
	for (_int i = _int(INVENTORYICON::ICON_TWO); i < _int(INVENTORYICON::ICON_END); ++i)
	{
	 	CSkill_Manager::PLAYERSKILL::SKILL eSkill =	CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON(i));

		if (eCurrentSkill == eSkill)
		{
			// �ش� ĭ�� ����.
			//CUI_Manager::GetInstance()->Set_IsInventoryIcon_Index(INVENTORYICON(i), false);
			
			m_iNextInventtoryIconIndex = _int(i) + 1;
			break;
		}
	}


	// �ش� ĭ�� ���� ���� �ƴ�, �ڿ� �ִ� �����͸� �� ĭ�� ������ ����, ���� �������� ����.
	for (_int j = m_iNextInventtoryIconIndex; j < _int(INVENTORYICON::ICON_END); ++j)
	{
		// ���� ������ �ϴ� �κ��丮 Index �� ������ �����Ͱ� �ִٸ�
		if (true == CUI_Manager::GetInstance()->Get_IsInventoryIcon_Index(INVENTORYICON(j)))
		{
			// �ش� �ε����� ������ �ִ� ��ų�� �����ͼ�
			CSkill_Manager::PLAYERSKILL::SKILL eSkill = CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON(j));
			// ���� ĭ�� ����ش�.
			CUI_Manager::GetInstance()->Set_InventoryIcon(INVENTORYICON(j - 1), eSkill);
		}
		else // ������ ĭ�� ���� �ݺ��� ������.
		{
			CUI_Manager::GetInstance()->Set_IsInventoryIcon_Index(INVENTORYICON(j - 1), false); 
			break;
		}
	}
}

void CSkill_Manager::Free()
{
}