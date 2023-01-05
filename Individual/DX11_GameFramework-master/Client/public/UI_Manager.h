#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"

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
	// 플레이어 레벨 게이지
	_float		Get_LevelGauge_Player() { return m_LevelGauge_Player; }
	void		Set_LevelGauge_Player(_float fGauge) { m_LevelGauge_Player = fGauge; }

	// 플레이어 체력 게이지
	_float		Get_HPGauge_Player() { return m_LevelGauge_Player; }
	void		Set_HPGauge_Player(_float fGauge) { m_LevelGauge_Player = fGauge; }

	// 몬스터 체력 게이지
	_float		Get_HPGauge_Monster() { return m_HpGauge_Monster; }
	void		Set_HPGauge_Monster(_float fGauge) { m_HpGauge_Monster = fGauge; }

	// UI 몬스터 인덱스
	_int		Get_UI_Monster_Index() { return m_iUI_Monster_Index; }
	void		CUI_Manager::UI_Monster_Index(CGameObject * pOther);

	// UI 몬스터 
	_bool		Get_UI_Monster() { return m_bUI_Monster; }
	void		Set_Ui_Monster(_bool bUI) { m_bUI_Monster = bUI; }

	// 대본
	_tchar*		Get_Text() { return m_szText; }
	void		Set_Text(const _tchar * szText) { memcpy(m_szText, szText, sizeof(_tchar[MAX_PATH])); }

	// 대화 창
	_bool		Get_Talk() { return m_bisTalk; }
	void		Set_Talk(_bool bTalk) { m_bisTalk = bTalk; }

private:
	_float			m_LevelGauge_Player = 0.0f;
	_float			m_HpGauge_Player = 1.0f;

	_float			m_HpGauge_Monster = 1.0f;
	_int			m_iUI_Monster_Index = 0;
	_bool			m_bUI_Monster = false;

	_tchar			m_szText[MAX_PATH] = L"";
	_bool			m_bisTalk = false;

public:
	virtual void Free() override;
};

END