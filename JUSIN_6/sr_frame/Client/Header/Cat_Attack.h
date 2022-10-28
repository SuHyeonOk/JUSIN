#pragma once
#include "SkeletalCube.h"

class CCat_Attack :	public CSkeletalCube
{
private:
	explicit CCat_Attack(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCat_Attack(const CCat_Attack& rhs);
	virtual ~CCat_Attack() override;

public:
	virtual HRESULT Ready_Object(const wstring& wstrPath);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Render_Object() override;
	static CCat_Attack* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath);

private:
	_float m_fSpeed = 0.f;
	_float m_fLifeTime = 4.f;
	_float m_fTime;
	_bool m_bDead = false;
	CubeAnimFrame m_Idle;
};

