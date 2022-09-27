#include "stdafx.h"
#include "..\Header\DynamicCamera.h"

#include "Export_Function.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCamera(pGraphicDev)
{
}

CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::Ready_Object(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_Object(), E_FAIL);

	return S_OK;
}

_int CDynamicCamera::Update_Object(const _float & fTimeDelta)
{
	Key_Input(fTimeDelta);
	_int iExit = CCamera::Update_Object(fTimeDelta);

	return iExit;
}

void CDynamicCamera::LateUpdate_Object(void)
{
	if (false == m_bFix)
	{
		Mouse_Fix();
		Mouse_Move();
	}
	CCamera::LateUpdate_Object();
}

void CDynamicCamera::Key_Input(const _float & fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, nullptr, &m_matView);

	if (Get_DIKeyState(DIK_T) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Get_DIKeyState(DIK_G) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Get_DIKeyState(DIK_H) & 0x80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Get_DIKeyState(DIK_F) & 0x80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Get_DIKeyState(DIK_T) & 0x80)
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}
	else
		m_bCheck = false;

	if (false == m_bFix)
		return;
}

void CDynamicCamera::Mouse_Move(void)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, nullptr, &m_matView);

	_long		dwMouseMove = 0;

	// 좌우 회전은 플레이어가 어떤 상태이던 간데 대부분 y축 기준으로 회전하게 됨
	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		_vec3		vUp{ 0.f, 1.f, 0.f };

		_vec3		vLook = m_vAt - m_vEye;

		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;

	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLook = m_vAt - m_vEye;

		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;

	}

}

void CDynamicCamera::Mouse_Fix(void)
{
	POINT pt{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CDynamicCamera * CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CDynamicCamera*		pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CDynamicCamera::Free(void)
{
	CCamera::Free();
}
