#pragma once
#include "SkeletalCube.h"
class CSkeletalGhostTrail : public CSkeletalCube
{
protected:
	explicit CSkeletalGhostTrail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkeletalGhostTrail(const CSkeletalGhostTrail& rhs);
	virtual ~CSkeletalGhostTrail();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	void SetColorTime(_float fLife, D3DXCOLOR BaseColor);
	void SetDead(){m_fLife = -1.f;}
	static CSkeletalGhostTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, CSkeletalCube* pSkeletal);

private:
	_float m_fLife = 0.f;
	_float m_fInitLife = 0.f;
	D3DXCOLOR m_BaseColor;
};

