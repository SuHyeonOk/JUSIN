#include "stdafx.h"
#include "..\public\Skill_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{
}

void	CSkill_Manager::Set_ChangeSkill_Create(_bool bIsCreate)					// 플레이어가 변신 상태인지
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
	if(false == CUI_Manager::GetInstance()->Get_IsIcon_Index(iIndex))			// 왼쪽 하단 스킬이 비어있다면 사용하지 못 한다.
		return;
	
	m_tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(iIndex);	// 현재 내가 누른 인덱스
	CSkill_Manager::GetInstance()->Set_Player_Skill(m_tPlayerSkill.eSkill);

	if(0 < m_arrPageCount[m_tPlayerSkill.eSkill])								// 예외처리 0보다 클때, 즉 스킬이 있을 때 사용 가능하다.
		m_arrPageCount[m_tPlayerSkill.eSkill] -= 1;								// 아이템 하나 감소

	if (0 >= m_arrPageCount[m_tPlayerSkill.eSkill])								// 스킬이 0개 이하이면...
	{
		// 스킬 창
		CUI_Manager::GetInstance()->Set_IsIcon_Index(iIndex, false);

		// 인벤토리
		Delete_InventoryIcon(m_tPlayerSkill.eSkill);
	}

	cout <<  "칸에 어떤 스킬이 있는지 : " << 0 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE) << " | " <<
			 1 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_TWO) << " | " <<
			 2 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_THREE) << " | " <<
			 3 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_FOUR) << endl;
}

void	CSkill_Manager::Page_PickUp(PLAYERSKILL::SKILL iIndex)
{
	if (PLAYERSKILL::SKILL::PAINT == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;			// 충돌한 객체의 개수 관리

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::PAINT])		// 아이템 개수가 1개 일 때만 스킬창에 추가할 수 있다.
		{
			SkillIcon(PLAYERSKILL::SKILL::PAINT);				// 비어있는 UI 창 확인해서 Icon 넣기
			InventoryIcon(PLAYERSKILL::SKILL::PAINT);			// 비어있는 인벤창 확인해서 Icon 넣기
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
		// JakeSon 의 경우에는 인벤토리에 이미 추가 되어 있기 때문에 함수를 호출하지 않는다.
	}

	//if (L"Item_Page_Paint" == pOther->Get_Tag())
	//{
	//	m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;			// 충돌한 객체의 개수 관리

	//	if(1 == m_arrPageCount[PLAYERSKILL::SKILL::PAINT])		// 아이템 개수가 1개 일 때만 스킬창에 추가할 수 있다.
	//		SkillIcon(PLAYERSKILL::SKILL::PAINT);				// 비어있는 UI 창 확인해서 Icon 넣기
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

	//cout << "스킬 개수 : " << 
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
		if (false == CUI_Manager::GetInstance()->Get_IsIcon_Index(ITEMINDEX(i)))	// 현재 IconIndex 가 비어있다면!
		{
			CUI_Manager::GetInstance()->Set_IsIcon_Index(ITEMINDEX(i), true);		// 해당 창을 그린다.
			CUI_Manager::GetInstance()->Set_SkillIcon(ITEMINDEX(i), eSkill);		// 그 곳에 내가 지금 획득한 아이템을 넣어라
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
			CUI_Manager::GetInstance()->Set_IsInventoryIcon_Index(INVENTORYICON(i), true);		// 해당 창을 그린다.
			CUI_Manager::GetInstance()->Set_InventoryIcon(INVENTORYICON(i), eSkill);			// 획득한 아이템을 넣는다.
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
			// 해당 칸을 비운다.
			//CUI_Manager::GetInstance()->Set_IsInventoryIcon_Index(INVENTORYICON(i), false);
			
			m_iNextInventtoryIconIndex = _int(i) + 1;
			break;
		}
	}


	// 해당 칸을 비우는 것이 아닌, 뒤에 있던 데이터를 한 칸씩 앞으로 당기고, 가장 마지막을 비운다.
	for (_int j = m_iNextInventtoryIconIndex; j < _int(INVENTORYICON::ICON_END); ++j)
	{
		// 현재 비우려고 하는 인벤토리 Index 의 다음에 데이터가 있다면
		if (true == CUI_Manager::GetInstance()->Get_IsInventoryIcon_Index(INVENTORYICON(j)))
		{
			// 해당 인덱스가 가지고 있는 스킬을 가져와서
			CSkill_Manager::PLAYERSKILL::SKILL eSkill = CUI_Manager::GetInstance()->Get_InventoryIcon(INVENTORYICON(j));
			// 이전 칸에 담아준다.
			CUI_Manager::GetInstance()->Set_InventoryIcon(INVENTORYICON(j - 1), eSkill);
		}
		else // 없으면 칸을 비우고 반복문 나간다.
		{
			CUI_Manager::GetInstance()->Set_IsInventoryIcon_Index(INVENTORYICON(j - 1), false); 
			break;
		}
	}
}

void CSkill_Manager::Free()
{
}