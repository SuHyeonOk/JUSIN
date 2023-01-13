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
	m_tPlayerSkill.eSkill = CUI_Manager::GetInstance()->Get_SkillIcon(iIndex);	// 현재 내가 누른 인덱스
	CSkill_Manager::GetInstance()->Set_Player_Skill(m_tPlayerSkill.eSkill);

	if(0 < m_arrPageCount[m_tPlayerSkill.eSkill])								// 예외처리 0보다 클때, 즉 스킬이 있을 때 사용 가능하다.
		m_arrPageCount[m_tPlayerSkill.eSkill] -= 1;								// 아이템 하나 감소

	if(0 >= m_arrPageCount[m_tPlayerSkill.eSkill])								// 스킬이 0개 이하이면 스킬창 에서 삭제한다.
		CUI_Manager::GetInstance()->Set_IsIcon_Index(iIndex, false);

	cout <<  "칸에 어떤 스킬이 있는지 : " << 0 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_ONE) << " | " <<
			 1 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_TWO) << " | " <<
			 2 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_THREE) << " | " <<
			 3 << "> " << CUI_Manager::GetInstance()->Get_SkillIcon(ITEM_FOUR) << endl;
}

void	CSkill_Manager::Page_PickUp(PLAYERSKILL::SKILL iIndex)
{
	if (PLAYERSKILL::SKILL::PAINT == iIndex)
	{
		m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;		// 충돌한 객체의 개수 관리

		if (1 == m_arrPageCount[PLAYERSKILL::SKILL::PAINT])		// 아이템 개수가 1개 일 때만 스킬창에 추가할 수 있다.
			SkillIcon(PLAYERSKILL::SKILL::PAINT);				// 비어있는 UI 창 확인해서 Icon 넣기
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
	//	m_arrPageCount[PLAYERSKILL::SKILL::PAINT] += 1;		// 충돌한 객체의 개수 관리

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

void CSkill_Manager::Free()
{
}