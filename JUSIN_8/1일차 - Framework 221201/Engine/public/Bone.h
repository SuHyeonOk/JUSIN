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

	// CBone
	void Set_OffsetMatrix(_float4x4 OffsetMatrix) {
		m_OffsetMatrix = OffsetMatrix;
	}

public:
	HRESULT Initialize(aiNode* pAINode);
	void Compute_CombindTransformationMatrix();

private: // CBone
	char				m_szName[MAX_PATH];			// 뼈의 이름
	_float4x4			m_OffsetMatrix;				// ﻿정점의 로컬 스페이스로 가기 위한 상태 변환 행렬
	_float4x4			m_TransformMatrix;			// ﻿뼈 만의 상태 변환 행렬
	_float4x4			m_CombindTransformMatrix;	// 내 행렬 * 부모 행렬 = 이 행렬
	CBone*				m_pParent;					// 뼈의 부모 저장
	

public:
	static CBone* Create(aiNode* pAINode);
	virtual void Free() override;
};

END