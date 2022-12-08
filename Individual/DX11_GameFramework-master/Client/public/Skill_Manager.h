#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CSkill_Manager : public CBase
{
	DECLARE_SINGLETON(CSkill_Manager)

public:
	CSkill_Manager();
	virtual ~CSkill_Manager() = default;

public:
	void	StunChick(_float3 f3Pos, _bool bStun = false);

private:
	_bool	bStrun_One = false;

public:
	virtual void Free() override;	
};

END