#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델의 교체가능한 한 파츠. */
/* 이 메시를 그려내기위한 정점, 인덱스 버퍼를 보유한다. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void SetUp_MeshBones(class CModel* pModel);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

private:
	aiMesh*				m_pAIMesh = nullptr;
	CModel::TYPE		m_eType;
	/* 이 메시는 m_iMaterialIndex번째 머테리얼을 사용한다. */
	_uint				m_iMaterialIndex = 0;

	/* 이 메시의 정점들에게 영향을 주는 뼈의 갯수. */
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;

private:
	HRESULT Ready_VertexBuffer_NonAnimModel(aiMesh* pAIMesh, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();
};

END