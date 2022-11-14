#pragma once

#include "Base.h"

class CMonster_Tool final : public CBase
{
public:
	CMonster_Tool();
	virtual ~CMonster_Tool() = default;

public:
	void	Tick_Monster_Tool();

public:
	virtual void Free() override;
};