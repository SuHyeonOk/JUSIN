#include "..\public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrices[eState]);	
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATE eState) const
{
	return m_TransformMatrices[eState];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const
{
	// 역 행렬은 Tick() 함수에서 계산
	return XMLoadFloat4x4(&m_TransformMatrices_Inverse[eState]);
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
}

void CPipeLine::Tick()
{
	// 역행렬을 구한다.
	XMStoreFloat4x4(&m_TransformMatrices_Inverse[D3DTS_VIEW], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_VIEW])));
	// 내가 주석 XMStoreFloat4x4(&m_TransformMatrices_Inverse[D3DTS_PROJ], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[D3DTS_PROJ])));

	// 구한 역행렬을 멤버변수 m_vCamPosition에 담는다. => 카메라의 위치
	memcpy(&m_vCamPosition, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}
