#pragma once

/* �̱���. ��� ������ �����Ѵ�.  */
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
	_matrix Get_TransformMatrix(TRANSFORMSTATE eState) const;			// ����뵵
	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eState) const;		// ���̴��� ������ �뵵
	_matrix Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const;	// ������� ����� �����ִ� �Լ�
	_float4	Get_CamPosition() { return m_vCamPosition; }				// ���� �߰�

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);	

public:
	void Tick();

private:
	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformMatrices_Inverse[D3DTS_END];
	_float4				m_vCamPosition;	// ī�޶��� ��ġ

public:
	virtual void Free() override;
	
};

END