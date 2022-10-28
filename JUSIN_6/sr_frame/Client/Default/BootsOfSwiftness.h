#pragma once
#include "EquipItem.h"
class CBootsOfSwiftness :
	public CEquipItem
{
private:
	explicit CBootsOfSwiftness(LPDIRECT3DDEVICE9 pGraphicDev);
	~CBootsOfSwiftness();

public:
	//gameobj function
	virtual HRESULT Ready_Object();
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CBootsOfSwiftness* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	// equipitem function
	virtual void Equipment(SkeletalPart* pSkeletalPart) override;
private:
	_float m_fLifeTime = 10.f;
	_float m_fAge = 0.f;
	_bool m_bEnd = false;


	_float m_fCurTrailTime = 0.15f;
	_float m_fTrailTime = 0.15f;
};
