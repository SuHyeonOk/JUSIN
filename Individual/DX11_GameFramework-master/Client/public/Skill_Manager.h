#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

public:
	typedef struct tagPlayerSkill
	{
		enum MAGIC { IDLE, RUN, MAGIC_END };

		MAGIC	eMagic;
	}PLAYERSKILL;

	typedef struct tagMonsterSkill
	{
		enum TREEWITCH { JUMP, PRESSURE, RISE, TREEWITCH_END };

		TREEWITCH		eTreeWitch;

	}MONSTERSKILL;

public:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	PLAYERSKILL					Get_Player_Skill() { return m_tPlayerSkill; }
	void						Set_Player_Skill(PLAYERSKILL::MAGIC eMagic) { m_tPlayerSkill.eMagic = eMagic; }

public:
	MONSTERSKILL				Get_Monster_Skill() { return m_tMonsterSkill; }
	MONSTERSKILL::TREEWITCH		Get_TreeWitch_Skill() { return m_tMonsterSkill.eTreeWitch; }
	void						Set_TreeWitch_Skill(MONSTERSKILL::TREEWITCH eTreeWitch) { m_tMonsterSkill.eTreeWitch = eTreeWitch; }

private:
	PLAYERSKILL		m_tPlayerSkill;
	MONSTERSKILL	m_tMonsterSkill;

public:
	void	StunChick(_float3 f3Pos, _bool bStun = false);

private:
	_bool	bStrun_One = false;

public:
	virtual void Free() override;	
};

END