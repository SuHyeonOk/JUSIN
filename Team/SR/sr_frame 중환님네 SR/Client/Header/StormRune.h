#pragma once
#include "Rune.h"
class CStormRune : public CRune
{
private:
	explicit CStormRune(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStormRune() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Collision() override;
	static CStormRune* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	virtual void EquipRune(CWeapon* pWeapon) override;
	virtual void UnEquipRune(CWeapon* pWeapon) override;
	virtual void Free() override;

private:
	_bool m_bFlame = false;

	_float m_fFlameTime = 0.4f;
	_float m_fCurFlameTime = 0.4f;
	SkeletalPart* m_WeaponTop = nullptr;
	SkeletalPart* m_WeaponBot = nullptr;

	vector<CLava_Particle*> m_vecFlame;

	CWeapon* m_pEquippedWeapon = nullptr;
};

