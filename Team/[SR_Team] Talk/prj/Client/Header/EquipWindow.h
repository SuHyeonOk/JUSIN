#pragma once
#include "UI.h"

class CEquipWindow : public CUI
{
private:
	explicit CEquipWindow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEquipWindow();

public:
	virtual HRESULT		Ready_Object(void)						override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void)					override;
	virtual void		Render_Obejct(void)						override;

private:
	virtual HRESULT		Add_Component(void);

public:
	static CEquipWindow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual void		Free(void);

private:
	_matrix				m_matView, m_matWorld;

private:
	_float					m_fTempPosX, m_fTempPosY = 2022.f;
	
};

