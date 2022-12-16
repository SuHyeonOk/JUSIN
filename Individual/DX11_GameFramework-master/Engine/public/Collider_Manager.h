#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)
public:
	enum COL_TYPE { COL_PLAYER, COL_P_WEAPON, COL_MONSTER, COL_M_WEAPON, 
		COL_FOOD, COL_COIN, COL_PAGE, COL_BULLET, COL_NPC, COL_END };

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Add_ColGroup(COL_TYPE eColType, class CGameObject* pGameObject);
	void	Update_Col(COL_TYPE eColType_L, COL_TYPE eColType_R);
	void	Reset_Col();

private:
	list<class CGameObject*>			m_ColliderObjects[COL_END];

public:
	virtual void Free() override;
};

END