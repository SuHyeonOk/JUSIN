#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_matrix Get_PivotMatrix() const {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}

	class CBone* Get_BonePtr(const char* pBoneName);

	void		Set_AnimIndex(_uint iAnimIndex, _bool	bRepetition = true);
	_bool		Get_Finished();		// 애니메이션이 끝났는지
	_int		Get_Keyframes();	// 현재 재생중인 애니메이션의 Frames
	_int		Get_AnimIndex() { return m_iCurrentAnimIndex; }
	_bool		Animation_Check(_int iCurrentAnimIndex);	// 현재 애니메이션 Index를 받아 현재 애니메이션과 동일하면 true 아니라면 false

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	void	Play_Animation(_double TimeDelta);
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName = nullptr, _uint iPassIndex = 0);

public:
	const aiScene*						m_pAIScene = nullptr;

	Assimp::Importer					m_Importer;
	TYPE								m_eType = TYPE_END;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint								m_iNumMeshes = 0;			// 몇 개의 메시로 구성되어 있는지
	vector<class CMesh*>				m_Meshes;

	_uint								m_iNumMaterials = 0;		// 재질 여러개
	vector<MODELMATERIAL>				m_Materials;

	/* 전체 뼈의 갯수. */
	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;					// 원형이 아닌 복제된 뼈를 가져야 한다.

	_bool								m_bRepetition = false;		// sh 애니메이션을 반복?
	_uint								m_iCurrentAnimIndex = 0;	// 현재 애니메이션 순서
	_uint								m_iNumAnimations = 0;		// 애니메이션 총 개수
	vector<class CAnimation*>			m_Animations;

	_float4x4							m_PivotMatrix;				// 모델 초기값 설정 행렬

private:
	_float								m_fBlendDuration = 0.2f;	// Blend
	_float								m_fBlendCurTime = 0.2f;
	_uint								m_iPreAnimIndex = 0;

public:
	HRESULT Ready_Bones(aiNode* pNode, class CBone* pParent);
	HRESULT Ready_MeshContainers();
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Animation();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix); // PivotMatrix : 내 모델의 초기 상태를 제어하기 위한 행렬
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END