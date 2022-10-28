#pragma once
#include "Weapon.h"
class CAxe : public CWeapon
{
private:
	explicit CAxe(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAxe();

public:
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	static CAxe* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual _int Attack() override;
	virtual void Equipment(SkeletalPart* pSkeletalPart) override;
	virtual void Collision() override;

	virtual void Free() override;
};


