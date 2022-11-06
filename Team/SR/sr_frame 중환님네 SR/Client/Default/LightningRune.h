#pragma once
#include "Rune.h"
class CLightningRune : public CRune
{
private:
	explicit CLightningRune(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLightningRune() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Collision() override;
	static CLightningRune* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	virtual void EquipRune(CWeapon* pWeapon) override;
	virtual void UnEquipRune(CWeapon* pWeapon) override;
	virtual void Free() override;

private:
	_bool m_bSpark = false;

	_float m_fSparkTime = 0.4f;
	_float m_fCurSparkTime = 0.4f;
	SkeletalPart* m_WeaponTop = nullptr;
	SkeletalPart* m_WeaponBot = nullptr;

	vector<CChainLightning*> m_vecSparks;

	CWeapon* m_pEquippedWeapon = nullptr;

	D3DLIGHT9 m_LightningLight;

	_float m_fLightTime = 0.2f;
	_float m_fCurLightTime = 0.2f;
};

