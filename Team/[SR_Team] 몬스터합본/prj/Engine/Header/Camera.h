#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;


public:
	void		Get_Eye(_vec3& pEye) { pEye = m_vEye; }
	void		Get_At(_vec3& pAt) { pAt = m_vAt; }
	void		Get_Up(_vec3& pUp) { pUp = m_vUp; }
	void		Get_ViewMatrix(_matrix& _viewMat) { _viewMat = m_matView; }
	void		Get_ProjectMatrix(_matrix& _ProjMat) { _ProjMat = m_matProj; }

protected:
	_vec3		m_vEye, m_vAt, m_vUp;
	_float		m_fFov, m_fAspect, m_fNear, m_fFar;
	_matrix		m_matView, m_matProj;

protected:
	virtual void		Free(void);
};

END

