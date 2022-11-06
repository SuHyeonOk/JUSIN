#pragma once
#include "Rune.h"
class CMultiShotRune : public CRune
{
private:
	explicit CMultiShotRune(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMultiShotRune() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Use() override;
	virtual void EquipRune(CWeapon* pWeapon) override;
	virtual void UnEquipRune(CWeapon* pWeapon) override;
	static CMultiShotRune* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void Free() override;

private:
	_uint m_iWeaponDamage = 0;
	CWeapon* m_pWeapon = nullptr;
};

