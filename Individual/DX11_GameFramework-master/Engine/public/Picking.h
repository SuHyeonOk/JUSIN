#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
public:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, GRAPHIC_DESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(); /* 월드스페이스 상의 마우스레이, 레이포스를 구한다. */
	void Compute_LocalRayInfo(_float4* pRayDir, _float4* pRayPos, class CTransform * pTransform);

private:
	class	CPipeLine*		m_pPipeLine = nullptr;

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	HWND					m_hWnd;
	_uint					m_iWinCX, m_iWinCY;

	_float4					m_vRayDir;
	_float4					m_vRayPos;

public:
	virtual void Free() override;
};

END