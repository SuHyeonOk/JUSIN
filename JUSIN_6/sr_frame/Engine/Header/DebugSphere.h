#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CDebugSphere : public CGameObject
{
private:
	explicit CDebugSphere(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDebugSphere(const CDebugSphere& rhs);
	virtual ~CDebugSphere() override;

public:
	HRESULT Ready_Object(const _vec3& vPos, _float fRadius);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object() override;
	virtual void Free() override;
	static CDebugSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _float fRadius, _float fTime = 1.f);

private:
	LPD3DXMESH m_pSphere = nullptr;
	_matrix m_matWorld;
	_float m_fTime = 0.f;
};

END