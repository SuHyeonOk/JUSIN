#pragma once
#include "Rune.h"
class CStunRune : public CRune
{
private:
	explicit CStunRune(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStunRune() override;

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	virtual void Free() override;
	virtual void Collision() override;
	static CStunRune* Create(LPDIRECT3DDEVICE9 pGraphicDev);
};

