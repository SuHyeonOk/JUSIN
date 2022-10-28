#pragma once
#include "SkeletalCube.h"
class CEndermanTrail : public CSkeletalCube
{
protected:
	explicit CEndermanTrail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEndermanTrail(const CEndermanTrail& rhs);
	virtual ~CEndermanTrail();

public:
	virtual HRESULT Ready_Object() override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	void SetColorTime(_float fLife, D3DXCOLOR BaseColor);

	static CEndermanTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, CSkeletalCube* pSkeletal, const vector<string>& vecExcludePart);
	

private:
	_float m_fLife = 0.f;
	_float m_fInitLife = 0.f;
	D3DXCOLOR m_BaseColor;

};

