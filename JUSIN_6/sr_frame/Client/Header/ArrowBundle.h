#pragma once
#include "ConsumeItem.h"
class CArrowBundle :
	public CConsumeItem
{
private:
	explicit CArrowBundle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrowBundle();

public:
	//gameobj function
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;

	static CArrowBundle* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

