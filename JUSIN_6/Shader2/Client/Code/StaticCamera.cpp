#include "stdafx.h"
#include "..\Header\StaticCamera.h"

#include "Export_Function.h"

CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{
}


CStaticCamera::~CStaticCamera()
{
}

HRESULT CStaticCamera::Ready_Object(const _vec3* pEye, 
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

Engine::_int CStaticCamera::Update_Object(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	Target_Renewal();

	_int iExit = CCamera::Update_Object(fTimeDelta);

	return iExit;
}

void CStaticCamera::LateUpdate_Object(void)
{

	CCamera::LateUpdate_Object();
}

void CStaticCamera::Free(void)
{
	CCamera::Free();
}

CStaticCamera* CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov /*= D3DXToRadian(60.f)*/, const _float& fAspect /*= (float)WINCX / WINCY*/, const _float& fNear /*= 0.1f*/, const _float& fFar /*= 1000.f*/)
{
	CStaticCamera*		pInstance = new CStaticCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CStaticCamera::Key_Input(const _float& fTimeDelta)
{
	if (Get_DIKeyState(DIK_W) & 0x80)
		m_fDistance -= fTimeDelta * m_fSpeed;
	
	if(Get_DIKeyState(DIK_S) & 0x80)
		m_fDistance += fTimeDelta * m_fSpeed;
	
	if (Get_DIKeyState(DIK_D) & 0x80)
		m_fAngle -= D3DXToRadian(180.f) * fTimeDelta;
	
	if (Get_DIKeyState(DIK_A) & 0x80)
		m_fAngle += D3DXToRadian(180.f) * fTimeDelta;

	
}

void CStaticCamera::Target_Renewal(void)
{
	CTransform*	pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"TestPlayer", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransform);

	_vec3	vLook;
	pPlayerTransform->Get_Info(INFO_LOOK, &vLook);

	m_vEye = vLook * -1.f;	// ¹æÇâ º¤ÅÍ
	D3DXVec3Normalize(&m_vEye, &m_vEye);

	m_vEye.y = 1.f;
	m_vEye *= m_fDistance;	// ¹æÇâ º¤ÅÍ

	_vec3		vRight;
	memcpy(&vRight, &pPlayerTransform->m_matWorld.m[0][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationAxis(&matRot, &vRight, m_fAngle);
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRot);

	m_vEye += pPlayerTransform->m_vInfo[INFO_POS];
	m_vAt = pPlayerTransform->m_vInfo[INFO_POS];
}
