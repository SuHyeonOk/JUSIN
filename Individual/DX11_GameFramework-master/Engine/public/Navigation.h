#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagCellInfo
	{
		_int		iCellType = 0;
		_float3		Point_A;
		_float3		Point_B;
		_float3		Point_C;

	}CELLINFO;

public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentIndex = -1;
		_int		iCellType = 0;

	}NAVIDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_int	Get_CellType();
	_int	Get_CellIndex();
	void	Set_CellIndex(_int iCellIndex);

	HRESULT	Cell_Create(CELLINFO tCellInfo);

public:
	_bool isMove_OnNavigation(_fvector TargetPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // 

private:
	vector<class CCell*>	m_Cells;
	NAVIDESC				m_NaviDesc;

private:
	HRESULT	Ready_Neighbor();

#ifdef _DEBUG
private:
	class CShader*			m_pShader = nullptr;
#endif // _DEBUG

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END