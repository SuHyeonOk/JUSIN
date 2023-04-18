#pragma once
#include "Base.h"
#include "Inventory.h"
#include "Logo.h"

class CInventory;
class CObjectStoreMgr : public CBase
{
	DECLARE_SINGLETON(CObjectStoreMgr)

private:
	explicit CObjectStoreMgr();
	virtual ~CObjectStoreMgr() override;

public:
	void SetPlayerName(const string& strPlayerName) { m_strPlayerName = strPlayerName; }
	void SetPlayerSkin(CharacterSelect eSkin);

	string GetPlayerName() { return m_strPlayerName;}
	Protocol::PlayerSkin GetPlayerSkin() { return m_eSkin; }
	virtual void Free() override;

	void StoreInventory(CInventory* pInventory) { m_pPlayerInven = pInventory; m_pPlayerInven->AddRef();}
	CInventory* GetInventory() { return m_pPlayerInven; }
private:
	string m_strPlayerName;
	Protocol::PlayerSkin m_eSkin = Protocol::PLAYER_TYPE_STEVE;
	CInventory* m_pPlayerInven = nullptr;
};

