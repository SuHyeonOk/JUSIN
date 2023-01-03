#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

public:
	typedef struct tagPlyaerSkill
	{
		enum SKILL { PAINT, MARCELINT, SKILL_END };
		SKILL		eSkill;

	}PLAYERSKILL;

	typedef struct tagPlayerMagicSkill
	{
		enum SKILLSTATE { IDLE, RUN, ATTACK, HIT, SKILLSTATE_END };
		SKILLSTATE		eSkill;

	}MAGICSKILL;

	//typedef struct tagMonsterSkill
	//{
	//	enum TREEWITCH { JUMP, PRESSURE, RISE, TREEWITCH_END };
	//	TREEWITCH		eTreeWitch;

	//}MONSTERSKILL;

public:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:		// 플레이어
	PLAYERSKILL					Get_Player_Skill() { return m_tPlayerSkill; }
	void						Set_Player_Skill(PLAYERSKILL::SKILL eSkillState) { m_tPlayerSkill.eSkill = eSkillState; }

	MAGICSKILL					Get_Magic_Skill() { return m_tMagicSkill; }
	void						Set_Magic_Skill(MAGICSKILL::SKILLSTATE eSkillState) { m_tMagicSkill.eSkill = eSkillState; }


public:		// 몬스터
	//MONSTERSKILL				Get_Monster_Skill() { return m_tMonsterSkill; }
	//MONSTERSKILL::TREEWITCH		Get_TreeWitch_Skill() { return m_tMonsterSkill.eTreeWitch; }
	//void						Set_TreeWitch_Skill(MONSTERSKILL::TREEWITCH eTreeWitch) { m_tMonsterSkill.eTreeWitch = eTreeWitch; }

private:	// 플레이어
	PLAYERSKILL		m_tPlayerSkill;
	MAGICSKILL		m_tMagicSkill;


private:	// 몬스터
	//MONSTERSKILL	m_tMonsterSkill;

public:
	virtual void Free() override;	
};

END