#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CObj_Manager : public CBase
{
	DECLARE_SINGLETON(CObj_Manager)

public:
	enum PLAYER { FINN, JAKE, FREE, RESET, PLAYER_END };

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	PLAYER	Get_Current_Player() { return m_eCurrent_Player; }

public:
	void	Tick(_double TimeDelta);

private:
	void	Current_Player();

private:
	// X 키를 누를 때 마다 Player 가 달라진다. 0:Finn / 2:Jake / 3:Free / 4:Reset->Finn
	_uint	m_ChangeTarget = 0;
	PLAYER	m_eCurrent_Player = FINN;

public:
	virtual void Free() override;	
};

END