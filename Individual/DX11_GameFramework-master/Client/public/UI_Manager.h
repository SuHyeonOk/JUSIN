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
	// �̴ϰ���
	_int Get_Snail_Distance() {
		return m_iSnail_Distance;
	}
	void	Set_Snail_Distance(_int fDistance) {
		m_iSnail_Distance = fDistance;
	}

	// ��
	_bool		Get_Shop() {
		return m_bisShop;
	}
	void		Set_Shop(_bool bShop) {
		m_bisShop = bShop;
	}

	// �κ��丮
	_bool		Get_IsInventoryIcon_Index(INVENTORYICON iIndex) {
		return m_bIsIcon_Index[_int(iIndex)];
	}
	void		Set_IsInventoryIcon_Index(INVENTORYICON iIndex, _bool bIsIndex) {
		m_bIsIcon_Index[_int(iIndex)] = bIsIndex;
	}

	CSkill_Manager::PLAYERSKILL::SKILL		Get_InventoryIcon(INVENTORYICON iIndex) {
		return m_arr_IconIndex[_int(iIndex)];
	}
	void		Set_InventoryIcon(INVENTORYICON iIndex, CSkill_Manager::PLAYERSKILL::SKILL iSkill) {
		m_arr_IconIndex[_int(iIndex)] = iSkill;
	}

	// ����ũ �ֱ�
	_int		Get_JakeSon_Count() {
		return m_iJakeSon_Count;
	}
	void		Set_JakeSon_Count() {
		++m_iJakeSon_Count;
	}

	// ��ų
	_bool		Get_IsIcon_Index(ITEMINDEX iIndex) { 
		return m_bIsSkill_Index[iIndex]; 
	}
	void		Set_IsIcon_Index(ITEMINDEX iIndex, _bool bIsIndex) { 
		m_bIsSkill_Index[iIndex] = bIsIndex;
	}

	CSkill_Manager::PLAYERSKILL::SKILL		Get_SkillIcon(ITEMINDEX iIndex) { 
		return m_arr_SkillIndex[iIndex];
	}
	void									Set_SkillIcon(ITEMINDEX iIndex, CSkill_Manager::PLAYERSKILL::SKILL iSkill) { 
		m_arr_SkillIndex[iIndex] = iSkill;
	}

	// ����
	_int		Get_Weapons() { return m_iUI_Weapons; }
	void		Set_Weapons(_int iIndex) { m_iUI_Weapons = iIndex; }

	_int		Get_Weapon_index() { 
		return m_iUI_Weapon_index; }
	void		Set_Weapon_index(_int iIndex) { 
		m_iUI_Weapon_index = iIndex; }

	// ���� ����
	_int		Get_Level_Number() { return m_iUI_Level_Number; }
	void		Set_Level_Number(_int iNumber) { m_iUI_Level_Number = iNumber; }

	// �÷��̾� ���� ������
	_float		Get_LevelGauge_Player() { return m_LevelGauge_Player; }
	void		Set_LevelGauge_Player(_float fGauge) { m_LevelGauge_Player = fGauge; }

	// �÷��̾� ü�� ������
	_float		Get_HPGauge_Player() { 
		return m_HpGauge_Player;
	}
	void		Set_HPGauge_Player(_float fGauge) { 
		m_HpGauge_Player = fGauge;
	}

	// ���� ü�� ������
	_float		Get_HPGauge_Monster() { 
		return m_HpGauge_Monster; 
	}
	void		Set_HPGauge_Monster(_float fGauge) { 
		m_HpGauge_Monster = fGauge;
	}

	// UI ���� �ε��� : �浹�� ���ʹ� �̰� �̴�.
	_int		Get_UI_Monster_Index() { 
		return m_iUI_Monster_Index;
	}
	void		CUI_Manager::UI_Monster_Index(CGameObject * pOther);

	// UI ���� : �� ���� ���Ͷ� �浹 �ؼ� UI �� ����.
	_bool		Get_UI_Monster() { 
		return m_bUI_Monster;
	}
	void		Set_Ui_Monster(_bool bUI) { 
		m_bUI_Monster = bUI;
	}

	// �뺻
	_tchar*		Get_Text() { return m_szText; }
	void		Set_Text(const _tchar * szText) { memcpy(m_szText, szText, sizeof(_tchar[MAX_PATH])); }

	// ��ȭ â
	_bool		Get_Talk() { return m_bisTalk; }
	void		Set_Talk(_bool bTalk) { m_bisTalk = bTalk; }

private:
	_int			m_iSnail_Distance = { 0 };		// �̴ϰ��� ������ �����̰� �������������� ���� �Ÿ�

	_bool			m_bisShop = false;

	_bool										m_bIsIcon_Index[_int(INVENTORYICON::ICON_END)] = { false };
	CSkill_Manager::PLAYERSKILL::SKILL			m_arr_IconIndex[_int(INVENTORYICON::ICON_END)] = { CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END };

	_bool										m_bIsSkill_Index[ITEMINDEX_END] = { false };
	CSkill_Manager::PLAYERSKILL::SKILL			m_arr_SkillIndex[ITEMINDEX_END] = { CSkill_Manager::PLAYERSKILL::SKILL::SKILL_END };

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