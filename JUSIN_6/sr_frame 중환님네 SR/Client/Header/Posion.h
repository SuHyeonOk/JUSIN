#pragma once
#include "ConsumeItem.h"
class CPosion :
	public CConsumeItem
{
private:
	explicit CPosion(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPosion();

public:
	//gameobj function
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CPosion* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

