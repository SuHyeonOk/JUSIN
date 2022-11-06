#pragma once
#include "Weapon.h"

class CCrossbow :
	public CWeapon
{
private:
	explicit CCrossbow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrossbow();

public:
	//gameobj function
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CCrossbow* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	// equipitem function
	virtual _int Attack() override;
	virtual void Equipment(SkeletalPart* pSkeletalPart) override;
	void LoadFireWork() { m_bFireWork = true; }
	_bool IsFireWork() const { return m_bFireWork; }

private:
	_bool m_bFireWork = false;
};

