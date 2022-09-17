#pragma once

#include "Camera.h"
#include "Engine_Include.h"

BEGIN(Engine)

class CTransform;

END

class CDynamicCamera : public Engine::CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

public:
	HRESULT Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, 
		const _float& fFov , 
		const _float& fAspect, 
		const _float& fNear, 
		const _float& fFar);

	virtual _int Update_Object(const _float& fTimeDelta) override;

	virtual void LateUpdate_Object(void) override;

public:
	static CDynamicCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

	HRESULT				Add_Component(void);
	void				Target_Renewal(void);
	
private:
	CTransform*			m_pTransCom = nullptr;

private:
	_bool			m_bFix = false;
	_bool			m_bCheck = true;

private:
	virtual void Free(void) override;

};

