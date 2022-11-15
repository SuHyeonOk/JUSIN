#pragma once

#include "Base.h"
#include "GameInstance.h"

#include "BackGround.h"

BEGIN(Engine)
class CGmaeInstance;
END

class CMonster_Tool final : public CBase
{
public:
	typedef struct tagMonsterInfoTexture
	{
		_tchar		wstrName;
		_int		iHp = 0;
		_float3		fPos = { 0.0f, 0.0f ,0.0f };

	}MINFOTEX;

public:
	CMonster_Tool();
	virtual ~CMonster_Tool() = default;

public:
	void	Tick_Monster_Tool();

private:
	CGameInstance*		m_pGameInstance = nullptr;

private:
	MINFOTEX			m_Temp;

	list<MINFOTEX*>		m_InfoList;

	_bool				m_bOneChake = false;

public:
	virtual void Free() override;
};