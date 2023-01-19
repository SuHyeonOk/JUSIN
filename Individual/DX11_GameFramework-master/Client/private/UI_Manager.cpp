#include "stdafx.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

void CUI_Manager::UI_Monster_Index(CGameObject * pOther)
{
	if (L"Monster__Ghost_0" == pOther->Get_Tag())
		m_iUI_Monster_Index = 0;
	else if (L"Monster__Ghost_1" == pOther->Get_Tag())
		m_iUI_Monster_Index = 1;
	else if (L"Monster__Ghost_2" == pOther->Get_Tag())
		m_iUI_Monster_Index = 2;
	else if (L"Monster__Skeleton_Archer" == pOther->Get_Tag() || L"Monster__Skeleton_Shield" == pOther->Get_Tag())
		m_iUI_Monster_Index = 3;
	else if (L"Monster__Gronmes_Blue" == pOther->Get_Tag())
		m_iUI_Monster_Index = 4;
	else if (L"Monster__Gronmes_Red" == pOther->Get_Tag())
		m_iUI_Monster_Index = 5;
	else if (L"Monster__Gronmes_Yellow" == pOther->Get_Tag())
		m_iUI_Monster_Index = 6;
	else if (L"Monster__Mimic" == pOther->Get_Tag())
		m_iUI_Monster_Index = 7;
	else if (L"Monster__PigWarrior_Bee" == pOther->Get_Tag())
		m_iUI_Monster_Index = 8;
	else if (L"Monster__Pigs_Cowboy" == pOther->Get_Tag())
		m_iUI_Monster_Index = 9;
	else if (L"Monster__Pigs_Sr" == pOther->Get_Tag())
		m_iUI_Monster_Index = 10;
	else if (L"Monster__PigWarrior_Worke" == pOther->Get_Tag())
		m_iUI_Monster_Index = 11;
	else if (L"Monster__TreeWitch" == pOther->Get_Tag())
		m_iUI_Monster_Index = 12;
	else if (L"Monster__Magic_Man" == pOther->Get_Tag())
		m_iUI_Monster_Index = 13;
	else if (L"Gary_Boss" == pOther->Get_Tag())
		m_iUI_Monster_Index = 14;
}

void CUI_Manager::Free()
{
}
