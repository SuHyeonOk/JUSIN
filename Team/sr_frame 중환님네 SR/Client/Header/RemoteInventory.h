#pragma once
#include "Inventory.h"
class CRemoteInventory : public CInventory
{
protected:
	explicit CRemoteInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRemoteInventory() = default;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	static CRemoteInventory* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void SetEquipStateChange(const Protocol::EquipState& state)
	{
		WRITE_LOCK;
		m_equipStateList.push_back(state);
	}

private:
	USE_LOCK;
	list<Protocol::EquipState> m_equipStateList;
};

