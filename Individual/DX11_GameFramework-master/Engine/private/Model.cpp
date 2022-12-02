#include "..\public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
{
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath)
{
	_uint			iFlag = 0;

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	///* 뼈. */
	//// 주축 (얘도 로드시에!)
	//m_pAIScene->mRootNode->mChildren->mChildren;

	//// 로드 시 한 번
	//m_pAIScene->mAnimations[0]->mChannels[0];

	//m_pAIScene->mMeshes[0]->mBones[0];





	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
{
	// pShader : 셰이더 컴포넌트
	// iMeshIndex : 어떤 메시가 쓰려고 하는 머티리얼 인지
	// eType : 어떤 텍스처 타입을 가져올지 (지금은 aiTextureType_DIFFUSE)
	// pConstantName : 셰이더 전역 변수 이름

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
	{
		MSG_BOX("Model does not have texture.");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex)
{
	pShader->Begin(0);

	if (nullptr != m_Meshes[iMeshIndex])
		m_Meshes[iMeshIndex]->Render(); // VIBuffer 의 자식으로 Render() 호출

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers() // 메시 추가하기 위한 함수
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes; // 메시의 개수

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pAIMesh = m_pAIScene->mMeshes[i]; // 실제 메시의 정보

		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, TYPE_ANIM, pAIMesh);
		if (nullptr == pMesh) // 메쉬가 잘 생성 되었니
			return E_FAIL;

		m_Meshes.push_back(pMesh); // 잘 생성 되었으니 push_back
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	// p_ pModelFilePath 는 경로를 쪼개기 위해 받아왔다. 
	char		szDirectory[MAX_PATH] = "";

	// s_ 4, 5인자 : szDirectory, MAX_PATH 디렉토리 경로
	// r_ 2, 3인자 : 드라이브 경로 nullptr 우리는 디렉토리 경로만 있으면 된다. 
	// q_ _splitpath_s() 1인자 : 누구를 쪼갤까? pModelFilePath 이자식 
	_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	/* 텍스쳐에 기록되어있는 픽셀단위 재질 정보를 로드한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials; // 0_ 머티리얼 개수 

	for (_uint i = 0; i < m_iNumMaterials; ++i) // 1_ 머티리얼 개수만큼 루프를 돌면서
	{
		// ﻿파일 이름, 패스, 확장자
		char		szTextureFileName[MAX_PATH] = "";
		char		szExt[MAX_PATH] = "";
		char		szTexturePath[MAX_PATH] = "";

		MODELMATERIAL			ModelMaterial; // 2_ 구조체를 채워준다.
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		// 8_ 0번 째 머티리얼 정보를 지역변수에 잠깐 담아두고, 왜? 
		// 7_ pAIMaterial 안 에는 머티리얼 개수가 있고, 머티리얼을 표현하기 위한 타입도 존재한다. 배열로! (메시외 똑같다.) 
		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j) // 5_ 현재 로드할 수 있는 최대 재질 개수
		{
			aiString			strTexturePath;

			// f_ reture 값 true : 경로 받아옴 / false : continue 
			// e_ GetTecture() 의 return 값은 aiReturn 로 FAILED 와 똑같으니 이용한다. 
			// d_ 3인자 [output] : strTexturePath 
			// c_ 2인자 [0] : 색이 다른 디퓨즈가 여러장 존재할 수 있어서 (ex_ 미니언 빨, 파>모델을 하나 두고 텍스처만 변경) 
			// b_ j 가 1 일때는 aiTextureType_DIFFUSE 이 있냐? 
			// a_ j == 0 => aiTextureType_NONE; j == 2 => aiTextureType_SPECULAR; j == 1 => aiTextureType_DIFFUSE; 
			// 9_ pAIMaterial 에는 GetTexture() 함수가 있고, 함수에게 이 텍스처가 있냐 물어본다. 
			// 4_ j 는 디퓨즈 맵 텍스처를 생성하는 형태 (디퓨즈가 있는지, 노말이 있는지를 위해 2중 for문)
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue; // g_ false 면 저장하지 말고 다음 텍스처 조사 
						  // h_ aiTextureType(j) 는 enum 값으로 casting 을 해준다 

						  // w_ 8, 9 인자 : szExt, MEX_PATH 확장자 
						  // v_ 6, 7인자, MAX_PATH : szTextureFileName 파일 이름 필요 
						  // u_ 1인자 : strTexturePath.data 받아온 경로에 데이터를 쪼갠다. 
						  // t_ 이 안으로 들어오면 진짜 Parh 를 만든다.
						  // k_ continue 에 걸리지 않고 여기 까지 온 것은 텍스처가 있었다는 것 이니 
						  // j_ strTexturePath.data 에서 data 를 보면 char 로 aistrrimp 이 담고 있는 문자열을 char 로 return 한다. strTexturePath.data == char 
						  // r_ _splitpath_s() assimp 에서 제공하는 경로를 받아오는 함수 
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);

			// x_ 뜯어낸 경로에 +경로 +파일이름 +확장자
			strcpy_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szTextureFileName);
			strcat_s(szTexturePath, szExt);

			// y_ 최종 경로를 _tchar 로 변경한다.
			_tchar			szFullPath[MAX_PATH] = TEXT("");

			// z_ seFullPath 에 경로가 담아진다.
			MultiByteToWideChar(CP_ACP, 0, szTexturePath, strlen(szTexturePath), szFullPath, MAX_PATH);

			// o_ 우리는 경로가 아닌 파일 이름만 알면된다 그래서 splitpath 이용하여 파일 이름만 잘라온다. 
			// n_ pModelFilePath 는 assimp 가 가지고 있는 재질 텍스처의 경로를 그대로 담아주는 녀석인데 
			// m_ Create 할 때 마지막으로 파일 경로를 넘겨주기 위해 인자로 받은 pModelFilePath 을 넘지기 못 한다.
			// l_ 구조체의 j 번째에 값을 채울 수 있다. 
			// 6_ 이렇게 구조체에 텍스처를 하나씩 다 채워줄 생각이다. 
			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

			// ㄱ_ ﻿이렇게 j로 루프를 돌면서 18개에 대한 텍스처 로드를 다 수행하고 다 끝나면 예외처리를 해준다.
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		// ㄴ_ ﻿이 루프가 끝나면 이 구조체가 여러가지 값의 텍스처를 다 채웠으니 이 구조체를 벡터에 담아준다. 끝.
		m_Materials.push_back(ModelMaterial); // 3_ 로드를 다 하면 구조체를 추가한다.
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath)))
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
