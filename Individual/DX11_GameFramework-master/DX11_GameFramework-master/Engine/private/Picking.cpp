#include "..\public\Picking.h"

#include "Transform.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
	: m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CPicking::Initialize(HWND hWnd, GRAPHIC_DESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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
	if (nullptr == m_pPipeLine)
		return;
	
	POINT			ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_float4			vMousePosTemp;

	/* 투영공간상의 마우스 좌표를 구한다. */
	vMousePosTemp.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePosTemp.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePosTemp.z = 0.f;
	vMousePosTemp.w = 1.f;

	_vector vMousePos = XMVectorSet(vMousePosTemp.x, vMousePosTemp.y, vMousePosTemp.z, 1.f);

	/* 뷰스페이스 상의 좌표를 구한다. */
	_matrix			ProjMatrixInv;
	ProjMatrixInv	= m_pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInv	= XMMatrixInverse(nullptr, ProjMatrixInv);
	vMousePos		= XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	/* 뷰스페이스 상의 마우스 레이와 레이포스를 구하자. */
	_vector vRayPos, vRayDir;
	m_vRayPos		= _float4(0.f, 0.f, 0.f, 1.f);
	vRayPos			= XMLoadFloat4(&m_vRayPos);
	vRayDir			= vMousePos - vRayPos;

	/* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */
	_matrix		ViewMatrixInv;
	ViewMatrixInv	= m_pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInv	= XMMatrixInverse(nullptr, ViewMatrixInv);
	vRayPos			= XMVector3TransformCoord(vRayPos, ViewMatrixInv);
	vRayDir			= XMVector3TransformNormal(vRayDir, ViewMatrixInv);

	XMStoreFloat4(&m_vRayPos, vRayPos);
	XMStoreFloat4(&m_vRayDir, vRayDir);
}

void CPicking::Compute_LocalRayInfo(_float4* Out_pRayDir, _float4* Out_pRayPos, CTransform * pTransform)
{
	_matrix		WorldMatrixInv = pTransform->Get_WorldMatrix_Inverse();

	_vector		vRayPos, vRayDir;
	vRayPos		= XMLoadFloat4(&m_vRayPos);
	vRayDir		= XMLoadFloat4(&m_vRayDir);

	vRayPos		= XMVector3TransformCoord(vRayPos, WorldMatrixInv);
	vRayDir		= XMVector3TransformNormal(vRayDir, WorldMatrixInv);

	XMStoreFloat4(&m_vRayPos, vRayPos);
	XMStoreFloat4(&m_vRayDir, vRayDir);

	*Out_pRayPos = m_vRayPos;
	*Out_pRayDir = m_vRayDir;
}

void CPicking::Free()
{
	Safe_Release(m_pPipeLine);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
