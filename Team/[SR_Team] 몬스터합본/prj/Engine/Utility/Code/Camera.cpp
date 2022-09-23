#include "..\..\Header\Camera.h"

#include "Export_Function.h"

USING(Engine)

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_Object(void)
{

	// 뷰스페이스 변환
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// 원근투영 변환
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
	
}

_int CCamera::Update_Object(const _float & fTimeDelta)
{
	// 뷰스페이스 변환은 매 프레임마다 해줘야 함(카메라의 위치나 방향이 바뀌니까)
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
	return 0;
}

void CCamera::LateUpdate_Object(void)
{
}

void CCamera::Free(void)
{
	CGameObject::Free();
}
