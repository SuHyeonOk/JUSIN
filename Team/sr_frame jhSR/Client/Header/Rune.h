#pragma once
#include "EquipItem.h"
#include "Weapon.h"

class CRune : public CEquipItem
{
protected:
	explicit CRune(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRune() override;

public:
	_uint GetTargetTypeMask() const { return m_eTargetType; }
	virtual HRESULT Ready_Object() override;
	virtual void EquipRune(CWeapon* pWeapon) {}
	virtual void UnEquipRune(CWeapon* pWeapon) {}

	_uint GetTargetType() { return m_eTargetType; }
	virtual void Free() override;
protected:
	_uint m_eTargetType = WEAPON_END;
};

