#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CObj_Manager : public CBase
{
	DECLARE_SINGLETON(CObj_Manager)

public:
	typedef struct tagPlayerInfo
	{
		enum PLAYER { FINN, JAKE, FREE, RESET, PLAYER_END };

		PLAYER	ePlayer = PLAYER_END;		// ���� �÷��̾�
		LEVEL	ePlayer_Level = LEVEL_END;	// ���� �÷��̾ �ִ� Level

	}PLAYERINFO;

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	PLAYERINFO	Get_Current_Player() { return m_ePlayerInfo; }
	_vector		Get_Player_Transform();

	void		Set_Plyaer_Level(LEVEL eLevelID) { m_ePlayerInfo.ePlayer_Level = eLevelID; }

public:
	void		Tick(_double TimeDelta);

private:
	void		Current_Player();	// ���� �÷��̾ �������� Tick �� ���� �Լ�

private:
	// X Ű�� ���� �� ���� Player �� �޶�����. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint			m_ChangeTarget = 0;
	PLAYERINFO		m_ePlayerInfo;

public:
	virtual void Free() override;	
};

END