#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)
public:
	enum COL_TYPE { COL_PLAYER, COL_MONSTER, COL_ITEM, COL_END };

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;


public:
	HRESULT Add_ColGroup(COL_TYPE eColType, class CGameObject* pGameObject);
	void Update_Col(COL_TYPE eColType_L, COL_TYPE eColType_R);
	void Reset_Col();

private:
	list<class CGameObject*>			m_RenderObjects[COL_END];

public:
	virtual void Free() override;
};

END