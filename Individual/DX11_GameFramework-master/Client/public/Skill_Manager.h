#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

public:
	typedef struct tagMonsterSkill
	{
		enum TREEWITCH { JUMP, PRESSURE, RISE, TREEWITCH_END };

		TREEWITCH		eTreeWitch;

	}MONSTERSKILL;

public:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	MONSTERSKILL	Get_Monster_Skill() { return m_tMonsterSkill; }
	MONSTERSKILL::TREEWITCH		Get_TreeWitch_Skill() { return m_tMonsterSkill.eTreeWitch; }
	void						Set_TreeWitch_Skill(MONSTERSKILL::TREEWITCH eTreeWitch) { m_tMonsterSkill.eTreeWitch = eTreeWitch; }

private:
	MONSTERSKILL	m_tMonsterSkill;

public:
	void	StunChick(_float3 f3Pos, _bool bStun = false);

private:
	_bool	bStrun_One = false;

public:
	virtual void Free() override;	
};

END