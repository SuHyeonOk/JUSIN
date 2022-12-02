#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_OffsetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombindMatrix() {
		return XMLoadFloat4x4(&m_CombindTransformMatrix);
	}

	void Set_OffsetMatrix(_float4x4 OffsetMatrix) {
		m_OffsetMatrix = OffsetMatrix;
	}

	void Set_TransformMatrix(_fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}

public:
	HRESULT Initialize(aiNode* pAINode);
	void Compute_CombindTransformationMatrix();

private: 
	char				m_szName[MAX_PATH];			// ���� �̸�
	_float4x4			m_OffsetMatrix;				// ?������ ���� �����̽��� ���� ���� ���� ��ȯ ���
	_float4x4			m_TransformMatrix;			// ?�� ���� ���� ��ȯ ���
	_float4x4			m_CombindTransformMatrix;	// �� ��� * �θ� ��� = �� ���
	CBone*				m_pParent = nullptr;		// ���� �θ� ����


public:
	static CBone* Create(aiNode* pAINode);
	virtual void Free() override;
};

END