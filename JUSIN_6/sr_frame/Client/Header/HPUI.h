#pragma once
#include "UI.h"
class CHPUI : public CUI
{
private:
	explicit CHPUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHPUI() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CHPUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum);
	void SetProgress(_int iCurHP, _uint iMaxHP, _int iDamage);



private:
	CRcTex* m_pHeartRed = nullptr;
	_uint m_iHeartRedTexNum = 0;

	CRcTex* m_pHeartWhite = nullptr;
	_uint m_iHeartWhiteTexture = 0;

	_bool m_bWorldSet = false;
	_matrix m_matHeartRedWorld;
	_matrix m_matHeartWhiteWorld;


	_float m_fProgress = 1.f;
	_float m_fFollowProgress = m_fProgress;

};

