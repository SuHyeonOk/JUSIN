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

HRESULT CStaticCamera::Ready_Object(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
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

_int CStaticCamera::Update_Object(const _float & fTimeDelta)
{
	//Key_Input(fTimeDelta);

	Target_Renewal();
	
	_int iExit = CCamera::Update_Object(fTimeDelta);
	return iExit;
}

void CStaticCamera::LateUpdate_Object(void)
{
	CCamera::LateUpdate_Object();
}

void CStaticCamera::Key_Input(const _float & fTimeDelta)
{
	if (Get_DIKeyState(DIK_W) & 0x80)	// 플레이어를 향해 줌인
		m_fDistance -= fTimeDelta * m_fSpeed;
	
	if (Get_DIKeyState(DIK_S) & 0x80)	// 줌아웃
		m_fDistance += fTimeDelta * m_fSpeed;

	if (Get_DIKeyState(DIK_D) & 0x80)	// (right 축 기준)회전
		m_fAngle -= D3DXToRadian(180.f) * fTimeDelta;

	if (Get_DIKeyState(DIK_A) & 0x80)
		m_fAngle += D3DXToRadian(180.f) * fTimeDelta;
}

void CStaticCamera::Target_Renewal(void)
{
	/*
	CTransform* pPlayerTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransCom);

	_vec3	vLook;
	pPlayerTransCom->Get_Info(INFO_LOOK, &vLook);

	m_vEye = vLook * -1.f;
	D3DXVec3Normalize(&m_vEye, &m_vEye);

	m_vEye.y = 1.f;		// 카메라의 높이 초기화
	m_vEye *= m_fDistance;

	_vec3	vRight;
	memcpy(&vRight, &pPlayerTransCom->m_matWorld.m[0][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationAxis(&matRot, &vRight, m_fAngle); // right 축에 대해 회전
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRot); // 회전행렬에 따라 m_vEye 방향벡터 회전

	m_vEye += pPlayerTransCom->m_vInfo[INFO_POS]; 
								// m_vEye = -vLook + PlayerPos ( vLook = PlayerPos - m_vEye)								 
	m_vAt = pPlayerTransCom->m_vInfo[INFO_POS]; // 카메라가 바라보는 위치m_vAt은 플레이어의 위치좌표
	*/


	CTransform*   pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransform);

	_vec3   vPos;
	pPlayerTransform->Get_Info(INFO_POS, &vPos);

	_vec3   vLook;
	pPlayerTransform->Get_Info(INFO_LOOK, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);

	m_vEye = vPos +0.3f * vLook;
	m_vAt = vPos + vLook;



}

CStaticCamera * CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CStaticCamera*		pInstance = new CStaticCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CStaticCamera::Free(void)
{
	CCamera::Free();
}
