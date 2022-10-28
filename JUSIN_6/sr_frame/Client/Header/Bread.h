#pragma once
#include "ConsumeItem.h"
class CBread :
	public CConsumeItem
{
private:
	explicit CBread(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBread();

public:
	//gameobj function
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CBread* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

