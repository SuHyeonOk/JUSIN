#pragma once
#include "ConsumeItem.h"
class CApple :
	public CConsumeItem
{
private:
	explicit CApple(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CApple();

public:
	//gameobj function
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CApple* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

