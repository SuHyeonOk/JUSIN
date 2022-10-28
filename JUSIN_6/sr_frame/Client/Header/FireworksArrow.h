#pragma once
#include "EquipItem.h"
class CFireworksArrow :
	public CEquipItem
{
private:
	explicit CFireworksArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFireworksArrow();

public:
	//gameobj function
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CFireworksArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	// equipitem function
	virtual void Equipment(SkeletalPart* pSkeletalPart) override;
};

