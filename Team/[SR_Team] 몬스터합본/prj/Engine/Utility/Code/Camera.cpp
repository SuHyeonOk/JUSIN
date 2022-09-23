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

	// �佺���̽� ��ȯ
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// �������� ��ȯ
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
	
}

_int CCamera::Update_Object(const _float & fTimeDelta)
{
	// �佺���̽� ��ȯ�� �� �����Ӹ��� ����� ��(ī�޶��� ��ġ�� ������ �ٲ�ϱ�)
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
