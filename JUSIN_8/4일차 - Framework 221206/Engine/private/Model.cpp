#include "..\public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	/*, m_Meshes(rhs.m_Meshes)*/
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	/*, m_Bones(rhs.m_Bones)*/
	, m_iNumBones(rhs.m_iNumBones)
	/*, m_Animations(rhs.m_Animations)*/
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	/*for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);*/

	//for (auto& pAnimation : m_Animations)
	//	Safe_AddRef(pAnimation);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pMesh : rhs.m_Meshes)
	{
		m_Meshes.push_back((CMesh*)pMesh->Clone());
	}
		

}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	// ���� ��ȸ �ϸ鼭 pBoneName �� ���� �༮�� ã�ƾ� �Ѵ�.
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;


	return nullptr;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	_uint			iFlag = 0;

	if(TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_eType = eType;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* ���� �ε��Ѵ�. / */
	/*if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;*/

	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;



 	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	for (auto& pMesh : m_Meshes)
	{
		pMesh->SetUp_MeshBones(this);
	}

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta)
{
	if (TYPE_NONANIM == m_eType)
		return;

	/* ���� �ִϸ��̼ǿ� �´� ������ TranformMAtrix�� �����ϳ�. */
	m_Animations[m_iCurrentAnimIndex]->Update_Bones(TimeDelta);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
	int a = 10;
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
{
	// pShader : ���̴� ������Ʈ
	// iMeshIndex : � �޽ð� ������ �ϴ� ��Ƽ���� ����
	// eType : � �ؽ�ó Ÿ���� �������� (������ aiTextureType_DIFFUSE)
	// pConstantName : ���̴� ���� ���� �̸�

	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pTexture[eType])
	{
		m_Materials[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShader, pConstantName);
	}
	else	
		return E_FAIL;	

	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName)
{
	if (nullptr != m_Meshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[128];

			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			
			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 128);
		}		

		pShader->Begin(0);	

		m_Meshes[iMeshIndex]->Render(); // VIBuffer �� �ڽ����� Render() ȣ��
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode * pAINode, CBone* pParent)
{
	/* �����ϰ����ؤ����� ���� ����. */
	CBone*		pBone = CBone::Create(pAINode, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	/* �̻��� �ڽĻ��� �����.  */
	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers() // �޽� �߰��ϱ� ���� �Լ�
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i) // �޽��� ����
	{
		aiMesh*		pAIMesh = m_pAIScene->mMeshes[i]; // ���� �޽��� ����

		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, pAIMesh, this);
		if (nullptr == pMesh)
			return E_FAIL; 

		m_Meshes.push_back(pMesh); // �޽ð� �� ���� �Ǿ����� push_back
	}	

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	// p_ pModelFilePath �� ��θ� �ɰ��� ���� �޾ƿԴ�. 
	char		szDirectory[MAX_PATH] = "";

	// s_ 4, 5���� : szDirectory, MAX_PATH ���丮 ���
	// r_ 2, 3���� : ����̺� ��� nullptr �츮�� ���丮 ��θ� ������ �ȴ�. 
	// q_ _splitpath_s() 1���� : ������ �ɰ���? pModelFilePath ���ڽ� 
	_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	/* �ؽ��Ŀ� ��ϵǾ��ִ� �ȼ����� ���� ������ �ε��Ѵ�. */
	m_iNumMaterials = m_pAIScene->mNumMaterials; // 0_ ��Ƽ���� ���� 

	for (_uint i = 0; i < m_iNumMaterials; ++i) // 1_ ��Ƽ���� ������ŭ ������ ���鼭
	{
		// ?���� �̸�, �н�, Ȯ����
		char		szTextureFileName[MAX_PATH] = "";
		char		szExt[MAX_PATH] = "";
		char		szTexturePath[MAX_PATH] = "";

		MODELMATERIAL			ModelMaterial; // 2_ ����ü�� ä���ش�.
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		// 8_ 0�� ° ��Ƽ���� ������ ���������� ��� ��Ƶΰ�, ��? 
		// 7_ pAIMaterial �� ���� ��Ƽ���� ������ �ְ�, ��Ƽ������ ǥ���ϱ� ���� Ÿ�Ե� �����Ѵ�. �迭��! (�޽ÿ� �Ȱ���.) 
		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j) // 5_ ���� �ε��� �� �ִ� �ִ� ���� ����
		{
			aiString			strTexturePath;

			// f_ reture �� true : ��� �޾ƿ� / false : continue 
			// e_ GetTecture() �� return ���� aiReturn �� FAILED �� �Ȱ����� �̿��Ѵ�. 
			// d_ 3���� [output] : strTexturePath 
			// c_ 2���� [0] : ���� �ٸ� ��ǻ� ������ ������ �� �־ (ex_ �̴Ͼ� ��, ��>���� �ϳ� �ΰ� �ؽ�ó�� ����) 
			// b_ j �� 1 �϶��� aiTextureType_DIFFUSE �� �ֳ�? 
			// a_ j == 0 => aiTextureType_NONE; j == 2 => aiTextureType_SPECULAR; j == 1 => aiTextureType_DIFFUSE; 
			// 9_ pAIMaterial ���� GetTexture() �Լ��� �ְ�, �Լ����� �� �ؽ�ó�� �ֳ� �����. 
			// 4_ j �� ��ǻ�� �� �ؽ�ó�� �����ϴ� ���� (��ǻ� �ִ���, �븻�� �ִ����� ���� 2�� for��)
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue; // g_ false �� �������� ���� ���� �ؽ�ó ���� 
						  // h_ aiTextureType(j) �� enum ������ casting �� ���ش� 

						  // w_ 8, 9 ���� : szExt, MEX_PATH Ȯ���� 
						  // v_ 6, 7����, MAX_PATH : szTextureFileName ���� �̸� �ʿ� 
						  // u_ 1���� : strTexturePath.data �޾ƿ� ��ο� �����͸� �ɰ���. 
						  // t_ �� ������ ������ ��¥ Parh �� �����.
						  // k_ continue �� �ɸ��� �ʰ� ���� ���� �� ���� �ؽ�ó�� �־��ٴ� �� �̴� 
						  // j_ strTexturePath.data ���� data �� ���� char �� aistrrimp �� ��� �ִ� ���ڿ��� char �� return �Ѵ�. strTexturePath.data == char 
						  // r_ _splitpath_s() assimp ���� �����ϴ� ��θ� �޾ƿ��� �Լ� 
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);

			// x_ �� ��ο� +��� +�����̸� +Ȯ����
			strcpy_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szTextureFileName);
			strcat_s(szTexturePath, szExt);

			// y_ ���� ��θ� _tchar �� �����Ѵ�.
			_tchar			szFullPath[MAX_PATH] = TEXT("");

			// z_ seFullPath �� ��ΰ� �������.
			MultiByteToWideChar(CP_ACP, 0, szTexturePath, strlen(szTexturePath), szFullPath, MAX_PATH);

			// o_ �츮�� ��ΰ� �ƴ� ���� �̸��� �˸�ȴ� �׷��� splitpath �̿��Ͽ� ���� �̸��� �߶�´�. 
			// n_ pModelFilePath �� assimp �� ������ �ִ� ���� �ؽ�ó�� ��θ� �״�� ����ִ� �༮�ε� 
			// m_ Create �� �� ���������� ���� ��θ� �Ѱ��ֱ� ���� ���ڷ� ���� pModelFilePath �� ������ �� �Ѵ�.
			// l_ ����ü�� j ��°�� ���� ä�� �� �ִ�. 
			// 6_ �̷��� ����ü�� �ؽ�ó�� �ϳ��� �� ä���� �����̴�. 
			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

			// ��_ ?�̷��� j�� ������ ���鼭 18���� ���� �ؽ�ó �ε带 �� �����ϰ� �� ������ ����ó���� ���ش�.
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		// ��_ ?�� ������ ������ �� ����ü�� �������� ���� �ؽ�ó�� �� ä������ �� ����ü�� ���Ϳ� ����ش�. ��.
		m_Materials.push_back(ModelMaterial); // 3_ �ε带 �� �ϸ� ����ü�� �߰��Ѵ�.
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*			pAnim = CAnimation::Create(pAIAnimation, this);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	m_Importer.FreeScene();
}
