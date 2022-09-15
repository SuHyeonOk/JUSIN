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
	D3DXMatrixIdentity(pMatrix); // 내보낼 pMatrix을 항등행렬로 만들기

	// 아래는 변형을 하기 위한 벡터
	_vec3	vRight	{ 1.f, 0.f, 0.f };
	_vec3	vUp		{ 0.f, 1.f, 0.f };
	_vec3	vLook	{ 0.f, 0.f, 1.f };
	_vec3	vPos	{ 0.f, 0.f, 0.f };

	// look 벡터를 만들자
	vLook = *pAt - *pEye; // 카메라가 바라보는 위치 - 카메라 위치 = 방향 벡터
	D3DXVec3Normalize(&vLook, &vLook);

	// right 벡터를 만들자

	// pUp과 vLook 내적 = vRight
	// vRight을 구하기 위해서는 pUP에서 vLook으로 (위에서 아래로)
	// 이 때 pUp는 실제 카메라가 가지고 있는 pUp벡터
	D3DXVec3Cross(&vRight, pUp, &vLook); 
	D3DXVec3Normalize(&vRight, &vRight);

	// up벡터를 만들자
	D3DXVec3Cross(&vUp, &vLook, &vRight); // vLook에서 vRight 외적 = vUp
	D3DXVec3Normalize(&vUp, &vUp);

	// pos벡터를 만들자
	vPos = *pEye;

	// 카메라의 월드 행렬 MakeTransformMatrix()함수를 이용해 만들기
	MakeTransformMatrix(pMatrix, &vRight, &vUp, &vLook, &vPos);

	// 뷰스페이스 변환 행렬
	D3DXMatrixInverse(pMatrix, 0, pMatrix);
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
