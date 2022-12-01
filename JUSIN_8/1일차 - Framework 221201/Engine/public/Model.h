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

	class CBone* Get_BonePtr(const char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg);

public: // CModel
	void Play_Animation(_double TimeDelta);
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);	
	HRESULT Render(CShader* pShader, _uint iMeshIndex);

public:
	const aiScene*						m_pAIScene = nullptr;
	Assimp::Importer					m_Importer;
	TYPE								m_eType = TYPE_END;

	/* �ϳ��� ���� ��ü�� ������ �������� �޽÷� �����Ǿ��ִ�. */
	_uint								m_iNumMeshes = 0; // �� ���� �޽÷� �����Ǿ� �ִ���
	vector<class CMesh*>				m_Meshes;	

	_uint								m_iNumMaterials = 0; // ���� ������
	vector<MODELMATERIAL>				m_Materials;

	/* ��ü ���� ����. */
	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;

	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

public: 
	HRESULT Ready_Bones(aiNode* pNode);
	HRESULT Ready_MeshContainers();
	HRESULT Ready_Materials(const char* pModelFilePath);
	/*HRESULT Ready_Animation();*/

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END