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
	_bool		Get_Finished();		// �ִϸ��̼��� ��������
	_int		Get_Keyframes();	// ���� ������� �ִϸ��̼��� Frames
	_int		Get_AnimIndex() { return m_iCurrentAnimIndex; }
	_bool		Animation_Check(_int iCurrentAnimIndex);	// ���� �ִϸ��̼� Index�� �޾� ���� �ִϸ��̼ǰ� �����ϸ� true �ƴ϶�� false

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

	/* �ϳ��� ���� ��ü�� ������ �������� �޽÷� �����Ǿ��ִ�. */
	_uint								m_iNumMeshes = 0;			// �� ���� �޽÷� �����Ǿ� �ִ���
	vector<class CMesh*>				m_Meshes;

	_uint								m_iNumMaterials = 0;		// ���� ������
	vector<MODELMATERIAL>				m_Materials;

	/* ��ü ���� ����. */
	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;					// ������ �ƴ� ������ ���� ������ �Ѵ�.

	_bool								m_bRepetition = false;		// sh �ִϸ��̼��� �ݺ�?
	_uint								m_iCurrentAnimIndex = 0;	// ���� �ִϸ��̼� ����
	_uint								m_iNumAnimations = 0;		// �ִϸ��̼� �� ����
	vector<class CAnimation*>			m_Animations;

	_float4x4							m_PivotMatrix;				// �� �ʱⰪ ���� ���

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
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix); // PivotMatrix : �� ���� �ʱ� ���¸� �����ϱ� ���� ���
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END