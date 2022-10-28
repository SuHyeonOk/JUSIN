#pragma once

#include "Camera.h"
#define PLANE_EPSILON	4.0f

extern class CDynamicCamera* g_cam;

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
	void	Set_Far(_float _far) {  m_fFar = _far;}

	_vec3 GetEys() const { return m_vEye; }

	BOOL	Make();

	BOOL	IsIn(_vec3* pv);

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

public:
	static CDynamicCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const _vec3* pEye, 
										const _vec3* pAt, 
										const _vec3* pUp,
										const _float& fFov = D3DXToRadian(60.f),
										const _float& fAspect = (float)WINCX / WINCY,
										const _float& fNear = 0.1f,
										const _float& fFar = 1000.f);
	_bool m_bStop = false;

private:
	_bool			m_bFix = false;
	_bool			m_bCheck = true;
	_vec3			m_vtx[8];	/// 프러스텀을 구성할 정점 8개
	_vec3			m_vPos;		/// 현재 카메라의 월드좌표
	D3DXPLANE		m_plane[6];	/// 프러스텀을 구성하는 6개의 평면


private:
	virtual void Free(void) override;

};

