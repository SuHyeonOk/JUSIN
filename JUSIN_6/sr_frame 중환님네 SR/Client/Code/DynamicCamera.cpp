#include "stdafx.h"
#include "..\Header\DynamicCamera.h"


CDynamicCamera* g_cam = nullptr;

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{
}


CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::Ready_Object(const _vec3* pEye, 
	const _vec3* pAt, 
	const _vec3* pUp,
	const _float& fFov,
	const _float& fAspect ,
	const _float& fNear,
	const _float& fFar)
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

Engine::_int CDynamicCamera::Update_Object(const _float& fTimeDelta)
{
	if (m_bStop) return OBJ_NOEVENT;

	Key_Input(fTimeDelta);
	_int iExit = CCamera::Update_Object(fTimeDelta);
	Make();

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

BOOL CDynamicCamera::Make()
{
	m_vtx[0].x = -1.0f;	m_vtx[0].y = -1.0f;	m_vtx[0].z = 0.0f;
	m_vtx[1].x = 1.0f;	m_vtx[1].y = -1.0f;	m_vtx[1].z = 0.0f;
	m_vtx[2].x = 1.0f;	m_vtx[2].y = -1.0f;	m_vtx[2].z = 1.0f;
	m_vtx[3].x = -1.0f;	m_vtx[3].y = -1.0f;	m_vtx[3].z = 1.0f;
	m_vtx[4].x = -1.0f;	m_vtx[4].y = 1.0f;	m_vtx[4].z = 0.0f;
	m_vtx[5].x = 1.0f;	m_vtx[5].y = 1.0f;	m_vtx[5].z = 0.0f;
	m_vtx[6].x = 1.0f;	m_vtx[6].y = 1.0f;	m_vtx[6].z = 1.0f;
	m_vtx[7].x = -1.0f;	m_vtx[7].y = 1.0f;	m_vtx[7].z = 1.0f;

	_matrix		matProjInv;
	//투영좌표의 역행렬
	D3DXMatrixInverse(&matProjInv, NULL, &m_matProj);

	for (_int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vtx[i], &m_vtx[i], &matProjInv);

	_matrix matViewInv;

	D3DXMatrixInverse(&matViewInv, NULL, &m_matView);

	for (_int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_vtx[i], &m_vtx[i], &matViewInv);

	// 0번과 5번은 프러스텀중 near평면의 좌측상단과 우측하단이므로, 둘의 좌표를 더해서 2로 나누면
	// 카메라의 좌표를 얻을 수 있다.(정확히 일치하는 것은 아니다.)
	m_vPos = (m_vtx[0] + m_vtx[5]) / 2.0f;

	// 얻어진 월드좌표로 프러스텀 평면을 만든다
	// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	D3DXPlaneFromPoints(&m_plane[0], m_vtx+4, m_vtx+7, m_vtx+6);	// 상 평면(top)
	D3DXPlaneFromPoints(&m_plane[1], m_vtx  , m_vtx+1, m_vtx+2);	// 하 평면(bottom)
	D3DXPlaneFromPoints(&m_plane[2], m_vtx  , m_vtx+4, m_vtx+5);	// 근 평면(near)
	D3DXPlaneFromPoints(&m_plane[3], m_vtx + 2, m_vtx + 6, m_vtx + 7);	// 원 평면(far)
	D3DXPlaneFromPoints(&m_plane[4], m_vtx, m_vtx + 3, m_vtx + 7);	// 좌 평면(left)
	D3DXPlaneFromPoints(&m_plane[5], m_vtx + 1, m_vtx + 5, m_vtx + 6);	// 우 평면(right)

	return TRUE;
}

BOOL CDynamicCamera::IsIn(_vec3 * pv)
{
	_float	fDist = 0.f;

	for(_int i = 0 ; i < 6 ; i++ )
	{
		fDist = D3DXPlaneDotCoord(&m_plane[i], pv);

		if (fDist > PLANE_EPSILON) 
			return FALSE;
	
	}

	return TRUE;
}

void CDynamicCamera::Free(void)
{
	CCamera::Free();
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov /*= D3DXToRadian(60.f)*/, const _float& fAspect /*= (float)WINCX / WINCY*/, const _float& fNear /*= 0.1f*/, const _float& fFar /*= 1000.f*/)
{
	CDynamicCamera*		pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}



void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, nullptr, &m_matView);

	if (Get_DIKeyState(DIK_W) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}
	
	if(Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (DIKeyDown(DIK_T))
	{
		m_bFix = !m_bFix;
	}

	// if (Get_DIKeyState(DIK_T) & 0x80)
	// {
	// 	if (m_bCheck)
	// 		return;
	//
	// 	m_bCheck = true;
	//
	// 	if (m_bFix)
	// 		m_bFix = false;
	// 	else
	// 		m_bFix = true;
	// }
	// else
	// 	m_bCheck = false;
	//
	// if (false == m_bFix)
	// 	return;
}

void CDynamicCamera::Mouse_Move(void)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, nullptr, &m_matView);

	_long		dwMouseMove = 0;

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
	POINT	pt{ WINCX >> 1 , WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}
