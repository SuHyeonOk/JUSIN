#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"

BEGIN(Client)

class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

	// 0 : 3���� �Ѿ�
	// 1 : ��������
	// 2 : ��
	// 3 : �ǿ���
	// 4 : ��������
	// 5 : �� ����ũ ��ü
	// 5 : Į

public:
	typedef struct tagPlyaerSkill
	{
		enum SKILL { PAINT, MARCELINT, COIN, FIONA, SKILL_END };
		SKILL		eSkill = SKILL_END;			// ���� ��� �� �� ��ų

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
	_bool						Get_ChangeSKill_Create() { return m_bChangeSkill_Create; }
	void						CSkill_Manager::Set_ChangeSkill_Create(_bool bIsCreate);

public:		// �÷��̾�
	PLAYERSKILL					Get_Player_Skill() { 
		return m_tPlayerSkill; }
	void						Set_Player_Skill(PLAYERSKILL::SKILL eSkillState) { 
		m_tPlayerSkill.eSkill = eSkillState; }

	MAGICSKILL					Get_Magic_Skill() { return m_tMagicSkill; }
	void						Set_Magic_Skill(MAGICSKILL::SKILLSTATE eSkillState) { m_tMagicSkill.eSkill = eSkillState; }

	FIONASKILL					Get_Fiona_Skill() { return m_tFionaSkill; }
	void						Set_Fiona_Skill(FIONASKILL::SKILLSTATE eSkillState) { m_tFionaSkill.eSkill = eSkillState; }

public:
	void		CSkill_Manager::Page_Use(ITEMINDEX	iIndex);
	void		CSkill_Manager::Page_PickUp(PLAYERSKILL::SKILL iIndex/*CGameObject * pOther*/);										// ���� ���� (�浹��) Page �� ���� ����

public:		// ������ ����
	_int		Get_PageCount(PLAYERSKILL::SKILL iIndex) { return m_arrPageCount[iIndex]; }				

	void		CSkill_Manager::SkillIcon(PLAYERSKILL::SKILL eSkill);										// IconIndex�� ����ִ��� Ȯ���ϰ�, �ε����� ���� ä���.

private:
	PLAYERSKILL		m_tPlayerSkill;
	MAGICSKILL		m_tMagicSkill;
	FIONASKILL		m_tFionaSkill;

	_int			m_arrPageCount[PLAYERSKILL::SKILL::SKILL_END] = { 0 };

	_bool			m_bChangeSkill_Create = false;

public:
	virtual void Free() override;	
};

END