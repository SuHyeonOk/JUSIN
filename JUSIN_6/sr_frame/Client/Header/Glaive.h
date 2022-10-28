#pragma once
#include "Weapon.h"
class CGlaive : public CWeapon
{
private:
	explicit CGlaive(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGlaive();

public:
	//gameobj function
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;	
	virtual void Free() override;

	static CGlaive* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	// equipitem function
	virtual _int Attack() override;
	virtual void Equipment(SkeletalPart* pSkeletalPart) override;
	virtual void Collision() override;
};

