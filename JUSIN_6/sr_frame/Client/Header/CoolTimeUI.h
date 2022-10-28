#pragma once
#include "UI.h"

enum class CoolTimeTarget
{
	ROLL,
	POTION,
	LEGACY1,
	LEGACY2,
	LEGACY3,
	COOL_END
};

class CCoolTimeUI : public CUI
{
private:
	explicit CCoolTimeUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCoolTimeUI() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CCoolTimeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum,  CoolTimeTarget eTarget);

private:
	CoolTimeTarget m_eTarget = CoolTimeTarget::COOL_END;
	CRcTex* m_pCoolTimeTex = nullptr;
	_uint m_iCoolTimeTexNum = 0;
	_matrix m_matCoolTimeWorld;


	_matrix m_matIconWorld;
	CTexture* m_pIconTexture = nullptr;
	_uint m_iIconTexNum = 0;

	_bool m_bWorldSet = false;

	_float m_fProgress = 1.f;
};

