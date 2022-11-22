#include "..\public\Picking.h"
#include "Transform.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::Initialize(HWND hWnd, GRAPHIC_DESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device*	pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_hWnd = hWnd;

	m_iWinCX = iWinCX;
	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	//POINT			ptMouse;

	//GetCursorPos(&ptMouse);

	//ScreenToClient(m_hWnd, &ptMouse);

	//_float3			vMousePos;

	///* ������������ ���콺 ��ǥ�� ���Ѵ�. */
	//vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	//vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	//vMousePos.z = 0.f;

	///* �佺���̽� ���� ��ǥ�� ���Ѵ�. */
	//_float4x4		ProjMatrixInv;
	//ProjMatrixInv = m_pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	//_matrix x = XMMatrixIdentity();
	//XMStoreFloat4x4(&ProjMatrixInv, x);
	////TODO����

	////m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	////D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	////D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

	///* �佺���̽� ���� ���콺 ���̿� ���������� ������. */
	//m_vRayPos = _float3(0.f, 0.f, 0.f);
	//m_vRayDir = vMousePos - m_vRayPos;

	///* ���彺���̽� ���� ���콺 ���̿� ���������� ������. */
	//_float4x4		ViewMatrixInv;
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	//D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
	//D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	//D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

}

void CPicking::Compute_LocalRayInfo(_float3 * pRayDir, _float3 * pRayPos, CTransform * pTransform)
{
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
