#include "..\..\Header\Calculator.h"
#include "Export_Function.h"

USING(Engine)

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CCalculator::CCalculator(const CCalculator & rhs)
	:CComponent(rhs)
{
}

CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator(void)
{
	return S_OK;
}

_float CCalculator::HeightOnTerrain(_vec3 * pPos, const _vec3 * pTerrainVtxPos, const _ulong & dwCntX, const _ulong & dwCntZ, const _ulong & dwVtxItv)
{
	_ulong dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float fWidth = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float fHeight = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	D3DXPLANE	Plane;	// 한평면을 정의하는 a,b,c,d 계수를 가지고 있음.
	// 법선벡터 n = { a, b, c }, 한 점 P = { x0, y0, z0 } 일때
	// 평면의 방정식은 a(x-x0) + b(y-y0) + c(z-z0) = 0
	// ax + by + cz + d = 0 // d = -(ax0 + by0 + cz0) = 0 (법선벡터라 내적하면 0)

	if (fWidth > fHeight)// 오른쪽 위 삼각형
	{
		// PLANE의 a,b,c,d를 구해주는 함수
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	else // 왼쪽 아래 삼각형
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	//pPos->y = (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;

	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;

}

CComponent * CCalculator::Clone(void)
{
	return new CCalculator(*this);
}

CCalculator * CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator*	pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
	{
		MSG_BOX("Calculator Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCalculator::Free(void)
{
	CComponent::Free();
}
