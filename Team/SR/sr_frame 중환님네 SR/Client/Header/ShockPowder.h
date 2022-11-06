#pragma once
#include "EquipItem.h"
class CShockPowder :
	public CEquipItem
{
private:
	explicit CShockPowder(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShockPowder();

public:
	//gameobj function
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CShockPowder* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void Use() { m_bUse = true; }
	// equipitem function
	virtual void Equipment(SkeletalPart* pSkeletalPart);
	
private:
	_bool m_bUse = false;
	_bool m_bColl = false;
};

