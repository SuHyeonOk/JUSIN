#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"

BEGIN(Client)

class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

	// 0 : 3방향 총알
	// 1 : 마르셀린
	// 2 : 돈
	// 3 : 피오나
	// 4 : 무지개콘
	// 5 : 핀 제이크 합체
	// 5 : 칼

public:
	typedef struct tagPlyaerSkill
	{
		enum SKILL { PAINT, MARCELINT, COIN, FIONA, FOOD, JAKESON, LOOK_JAKESON, SKILL_END };
		SKILL		eSkill = SKILL_END;			// 현재 사용 중 인 스킬

	}PLAYERSKILL;

	typedef struct tagPlayerMagicSkill
	{
		enum SKILLSTATE { IDLE, RUN, ATTACK, HIT, SKILLSTATE_END };
		SKILLSTATE		eSkill = SKILLSTATE_END;

	}MAGICSKILL;

	typedef struct tagPlayerFionaSkill
	{
		enum SKILLSTATE { IDLE, RUN, ATTACK, CAT, HIT, STUN, DANCE, SKILLSTATE_END };
		SKILLSTATE		eSkill = SKILLSTATE_END;

	}FIONASKILL;

public:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	_bool						Get_ChangeSkill_Create() { return m_bChangeSkill_Create; }
	void						CSkill_Manager::Set_ChangeSkill_Create(_bool bIsCreate);

public:		// 플레이어
	PLAYERSKILL					Get_Player_Skill() { 
		return m_tPlayerSkill; }
	void						Set_Player_Skill(PLAYERSKILL::SKILL eSkillState) { 
		m_tPlayerSkill.eSkill = eSkillState; }

	MAGICSKILL					Get_Magic_Skill() { 
		return m_tMagicSkill;
	}
	void						Set_Magic_Skill(MAGICSKILL::SKILLSTATE eSkillState) {
		m_tMagicSkill.eSkill = eSkillState; 
	}

	FIONASKILL					Get_Fiona_Skill() { return m_tFionaSkill; }
	void						Set_Fiona_Skill(FIONASKILL::SKILLSTATE eSkillState) { m_tFionaSkill.eSkill = eSkillState; }

public:
	void		CSkill_Manager::Skill_Sound(CSkill_Manager::PLAYERSKILL::SKILL eSkill);
	void		CSkill_Manager::Page_Use(ITEMINDEX	iIndex);
	void		CSkill_Manager::Page_PickUp(PLAYERSKILL::SKILL iIndex/*CGameObject * pOther*/);										// 현재 먹은 (충돌한) Page 의 종류 관리

public:		// 유아이 관련
	_int		Get_PageCount(PLAYERSKILL::SKILL iIndex) { 
		return m_arrPageCount[iIndex];
	}		
	void		CSkill_Manager::Set_Page_Penny(PLAYERSKILL::SKILL iIndex);

	void		CSkill_Manager::SkillIcon(PLAYERSKILL::SKILL eSkill);										// IconIndex가 비어있는지 확인하고, 인덱스에 값을 채운다.
	void		CSkill_Manager::InventoryIcon(PLAYERSKILL::SKILL eSkill);
	void		CSkill_Manager::Delete_InventoryIcon(PLAYERSKILL::SKILL eCurrentSkill);

private:
	PLAYERSKILL		m_tPlayerSkill;
	MAGICSKILL		m_tMagicSkill;
	FIONASKILL		m_tFionaSkill;

	_int			m_arrPageCount[PLAYERSKILL::SKILL::SKILL_END] = { 0 };

	_bool			m_bChangeSkill_Create = false;
	_int			m_iNextIndex = 0;

public:
	virtual void Free() override;	
};

END