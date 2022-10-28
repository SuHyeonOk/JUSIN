#pragma once
#include "Base.h"

struct DamageFont
{
	_int iDamage = 0;
	_vec2 vDir;
	_vec2 vPos;
	_float fTime = 0.5f;
	D3DXCOLOR Color;
	_bool bCritical;
	DamageFont(_int i_damage, const _vec2& v_dir, const _vec2& v_pos, const D3DXCOLOR& color, _bool bCritical)
		: iDamage(i_damage),
		  vDir(v_dir),
		  vPos(v_pos),
		  Color(color),
          bCritical(bCritical)
	{
	}
};

class CDamageFontMgr : public CBase
{
	DECLARE_SINGLETON(CDamageFontMgr);
private:
	explicit CDamageFontMgr();
	virtual ~CDamageFontMgr() override;

public:
	void SetGraphicDev(LPDIRECT3DDEVICE9 pGraphicDev) {m_pGraphicDev = pGraphicDev; m_pGraphicDev->AddRef();};
	void Update_DamageFontMgr(const _float& fTimeDelta);
	void Add_DamageFontFromWorld(_int iDamage, const _vec3& vPos, const _vec3& vFrom, D3DXCOLOR Color, _bool bCritical = false);
	void Add_DamageFontFromScreen(_int iDamage, const _vec2& vScreen, D3DXCOLOR Color);
	void Render_DamageFontMgr();

public:
	virtual void Free() override;

private:
	LPDIRECT3DDEVICE9 m_pGraphicDev;
	vector<DamageFont> m_vecDamageFont;
	_float m_fSpeed;

	_matrix m_matView;
	_matrix m_matProj;
	D3DVIEWPORT9 m_ViewPort;
};

