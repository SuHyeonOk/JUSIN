#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_float3 Get_Point(POINT ePoint) {
		return m_vPoints[ePoint];
	}

	void Set_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) {
		m_iNeighborIndices[eNeighbor] = pNeighbor->m_iIndex;
	}

	_int	Get_CellType() { 
		return m_iCellType;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex, _int iNaviType);

public:
	_bool Compare_Points(const _float3& SourPoint, const _float3& DestPoint);
	_bool isIn(_fvector vTargetPos, _int* pNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	_int				m_iIndex = 0;
	_float3				m_vPoints[POINT_END];
	_int				m_iNeighborIndices[NEIGHBOR_END];
	
	_int				m_iCellType = 0;

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = nullptr;
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, _int iNaviType);
	virtual void Free();
};

END