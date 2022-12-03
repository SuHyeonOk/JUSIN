#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CObj_Manager : public CBase
{
	DECLARE_SINGLETON(CObj_Manager)

public:
	CObj_Manager();
	virtual ~CObj_Manager() = default;

public:
	virtual void Free() override;	
};

END