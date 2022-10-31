#pragma once

#include "Camera.h"
#include "Engine_Include.h"

class CStaticCamera : public Engine::CCamera
{
private:
	explicit CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticCamera();

public:
	HRESULT Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, 
		const _float& fFov , 
		const _float& fAspect, 
		const _float& fNear, 
		const _float& fFar);

	virtual _int Update_Object(const _float& fTimeDelta) override;

	virtual void LateUpdate_Object(void) override;

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Target_Renewal(void);

private:
	_float			m_fDistance = 10.f;
	_float			m_fSpeed = 10.f;
	_float			m_fAngle = 0.f;

public:
	static CStaticCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const _vec3* pEye, 
										const _vec3* pAt, 
										const _vec3* pUp,
										const _float& fFov = D3DXToRadian(60.f),
										const _float& fAspect = (float)WINCX / WINCY,
										const _float& fNear = 0.1f,
										const _float& fFar = 1000.f);

private:
	virtual void Free(void) override;

};

