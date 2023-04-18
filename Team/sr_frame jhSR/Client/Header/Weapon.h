#pragma once
#include "EquipItem.h"

enum WeaponType
{
	WEAPON_SWORD = 1,
	WEAPON_GLAIVE = 1 << 1,
	WEAPON_AXE = 1 << 2,
	WEAPON_CROSSBOW = 1 << 3,
	WEAPON_END
};

class CRune;
class CInventoryUI;
class CInventory;

class CWeapon : public CEquipItem
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeapon() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	bool SetRune(CRune* pRune);
	CRune* GetRune() { return m_pRune; }
	_int GetWeaponType() { return m_eType; }
	void SetDamage(_uint iDamage) { m_iDamage = iDamage;}
	_uint GetDamage() const { return m_iDamage; }
	void SetCritical(_bool bCritical) {m_bCritical = bCritical;}

	_bool IsEquipped() const { return m_bEquipped; }
	void SetEquip(_bool bEquipped)
	{
		m_bEquipped = bEquipped;
		if (m_bEquipped == false)
			m_iAttackCnt = 0;
	}
	void SetInventory(CInventory* pInventory) { m_pInventory = pInventory; }

	virtual void Free() override;

protected:
	CRune* m_pRune;
	_int m_eType = WEAPON_END;
	_uint m_iDamage = 0;
	_bool m_bCritical = false;

	CInventory* m_pInventory;

	_bool m_bEquipped = false;
};

