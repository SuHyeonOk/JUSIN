#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"
#include "Skill_Manager.h"

BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	typedef struct tagUITextInfo
	{
		enum NPCTEXT { DOCTOR, NPCTEXT_END };

		NPCTEXT		eNpcText = NPCTEXT_END;
		_tchar		szText[MAX_PATH];

	}TEXTINFO;

public:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	// 제이크 애기
	_int		Get_JakeSon_Count() {
		return m_iJakeSon_Count;
	}
	void		Set_JakeSon_Count() {
		++m_iJakeSon_Count;
	}

	// 스킬
	_bool		Get_IsIcon_Index(ITEMINDEX iIndex) { return m_bIsSkill_Index[iIndex]; }
	void		Set_IsIcon_Index(ITEMINDEX iIndex, _bool bIsIndex) { m_bIsSkill_Index[iIndex] = bIsIndex; }

	CSkill_Manager::PLAYERSKILL::SKILL		Get_SkillIcon(ITEMINDEX iIndex) { 
		return m_iUI_Skill_Index[iIndex];
	}
	void									Set_SkillIcon(ITEMINDEX iIndex, CSkill_Manager::PLAYERSKILL::SKILL iSkill) { 
		m_iUI_Skill_Index[iIndex] = iSkill;
	}

	// 무기
	_int		Get_Weapons() { return m_iUI_Weapons; }
	void		Set_Weapons(_int iIndex) { m_iUI_Weapons = iIndex; }

	_int		Get_Weapon_index() { 
		return m_iUI_Weapon_index; }
	void		Set_Weapon_index(_int iIndex) { 
		m_iUI_Weapon_index = iIndex; }

	// 레벨 숫자
	_int		Get_Level_Number() { return m_iUI_Level_Number; }
	void		Set_Level_Number(_int iNumber) { m_iUI_Level_Number = iNumber; }

	// 플레이어 레벨 게이지
	_float		Get_LevelGauge_Player() { return m_LevelGauge_Player; }
	void		Set_LevelGauge_Player(_float fGauge) { m_LevelGauge_Player = fGauge; }

	// 플레이어 체력 게이지
	_float		Get_HPGauge_Player() { 
		return m_HpGauge_Player;
	}
	void		Set_HPGauge_Player(_float fGauge) { 
		m_HpGauge_Player = fGauge;
	}

	// 몬스터 체력 게이지
	_float		Get_HPGauge_Monster() { return m_HpGauge_Monster; }
	void		Set_HPGauge_Monster(_float fGauge) { 
		m_HpGauge_Monster = fGauge;
	}

	// UI 몬스터 인덱스
	_int		Get_UI_Monster_Index() { return m_iUI_Monster_Index; }
	void		CUI_Manager::UI_Monster_Index(CGameObject * pOther);

	// UI 몬스터 
	_bool		Get_UI_Monster() { 
		return m_bUI_Monster;
	}
	void		Set_Ui_Monster(_bool bUI) { 
		m_bUI_Monster = bUI;
	}

	// 대본
	_tchar*		Get_Text() { return m_szText; }
	void		Set_Text(const _tchar * szText) { memcpy(m_szText, szText, sizeof(_tchar[MAX_PATH])); }

	// 대화 창
	_bool		Get_Talk() { return m_bisTalk; }
	void		Set_Talk(_bool bTalk) { m_bisTalk = bTalk; }

private:
	_bool										m_bIsSkill_Index[ITEMINDEX_END] = { false };
	CSkill_Manager::PLAYERSKILL::SKILL			m_iUI_Skill_Index[ITEMINDEX_END] = { CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END };

	_int			m_iUI_Weapon_index = 0;
	_int			m_iUI_Weapons = 0;

	_int			m_iUI_Level_Number = 0;

	_float			m_LevelGauge_Player = 0.0f;
	_float			m_HpGauge_Player = 1.0f;

	_float			m_HpGauge_Monster = 1.0f;
	_int			m_iUI_Monster_Index = 0;
	_bool			m_bUI_Monster = false;

	_int			m_iJakeSon_Count = 0;

	_tchar			m_szText[MAX_PATH] = L"";
	_bool			m_bisTalk = false;

public:
	virtual void Free() override;
};

END