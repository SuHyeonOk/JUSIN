#pragma once

#include "Client_Defines.h"
#include "Base.h"

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
	void		CUI_Manager::Add_Text(TEXTINFO::NPCTEXT eNpcText, const _tchar & cText);

	_bool		Get_Talk() { return m_bisTalk; }
	void		Set_Talk(_bool bTalk) { m_bisTalk = bTalk; }

private:
	vector<TEXTINFO>			m_vecTalkText;
	_bool						m_bisTalk = false;

public:
	virtual void Free() override;
};

