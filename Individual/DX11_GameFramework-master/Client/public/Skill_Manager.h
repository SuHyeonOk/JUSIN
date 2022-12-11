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
		enum HUGWOLF { BUG, MISS, HUGWOLF_END };

		HUGWOLF		eHugWolf;

	}MONSTERSKILL;

public:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	MONSTERSKILL	Get_MonsterSkill() { return m_tMonsterSkill; }
	void			Set_MonsterSkill(MONSTERSKILL::HUGWOLF eHugWolf) { m_tMonsterSkill.eHugWolf = eHugWolf; }


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