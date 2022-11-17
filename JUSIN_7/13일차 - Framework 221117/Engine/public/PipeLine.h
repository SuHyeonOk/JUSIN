#pragma once

/* 싱글톤. 뷰랑 투영을 보관한다.  */
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix Get_TransformMatrix(TRANSFORMSTATE eState) const;			// 연산용도
	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eState) const;		// 셰이더에 던지는 용도
	_matrix Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const;	// 역행렬을 계산해 던져주는 함수
	_float4	Get_CamPosition() { return m_vCamPosition; }				// 내가 추가

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);	

public:
	void Tick();

private:
	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformMatrices_Inverse[D3DTS_END];
	_float4				m_vCamPosition;	// 카메라의 위치

public:
	virtual void Free() override;
	
};

END