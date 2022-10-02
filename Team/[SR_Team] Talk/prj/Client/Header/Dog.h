#pragma once
#include "NPC.h"

class CDog : public CNPC
{
private:
	explicit CDog(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDog();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void) override;
	virtual void		OnText();

private:
	// Timer
	_float				m_fTimeAcc;

public:
	static CDog*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);
};


