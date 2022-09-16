#include "..\..\Header\Pipeline.h"

CPipeline::CPipeline()
{
}


CPipeline::~CPipeline()
{
}

void CPipeline::MakeTransformMatrix(_matrix * pMatrix, const _vec3 * pRight, const _vec3 * pUp, const _vec3 * pLook, const _vec3 * pPos)
{
	memcpy(&pMatrix->m[0][0], pRight, sizeof(_vec3));
	memcpy(&pMatrix->m[1][0], pUp, sizeof(_vec3));
	memcpy(&pMatrix->m[2][0], pLook, sizeof(_vec3));
	memcpy(&pMatrix->m[3][0], pPos, sizeof(_vec3));
}


void CPipeline::MakeViewMatirx(_matrix * pMatrix, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp)
{
	// 뷰 스페이스 변환 행렬
	D3DXMatrixIdentity(pMatrix);

	_vec3 vRight	{ 1.f, 0.f, 0.f };
	_vec3 vUp		{ 0.f, 1.f, 0.f };
	_vec3 vLook		{ 0.f, 0.f, 1.f };
	_vec3 vPos		{ 1.f, 0.f, 0.f };

	vLook = *pAt - *pEye;
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, pUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVector


}

void CPipeline::MakeProjMatirx(_matrix * pMatrix, const _float & fFov, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	D3DXMatrixIdentity(pMatrix); // 내보낼 pMatrix을 항등행렬로 만들기

	pMatrix->_11 = (1.f / tanf(fFov / 2.f)) / fAspect; // 스케일값 / 종횡비
	pMatrix->_22 = 1.f / tanf(fFov / 2.f);
	
	pMatrix->_33 = fFar / (fFar - fNear);
	pMatrix->_34 = 1.f;
	
	pMatrix->_43 = (-fNear * fFar) / (fFar - fNear);
	pMatrix->_44 = 0.f;
}
