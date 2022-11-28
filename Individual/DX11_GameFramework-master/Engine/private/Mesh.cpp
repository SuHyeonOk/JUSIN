#include "..\public\Mesh.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, aiMesh * pAIMesh)
{
	m_eType = eType;

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;		// 버텍스 버퍼 하나
	m_iNumVertices = pAIMesh->mNumVertices;	// mNumVertices 정점 개수
	m_iNumPrimitive = pAIMesh->mNumFaces;	// mNumFaces 면 개수
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;	// 개수가 많으니 32 사용
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32); // 32
	m_iNumIndicesPerPrimitive = 3; // 도형하나그릴 때 사용하는 정점 개수
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive; // 정점 개수 * 삼각형 개수

#pragma region VERTEX_BUFFER

	if (CModel::TYPE_NONANIM == m_eType)
	{
		Ready_VertexBuffer_NonAnimModel(pAIMesh);
	}
	else
	{
		Ready_VertexBuffer_AnimModel(pAIMesh);
	}



#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(aiMesh * pAIMesh)
{
	m_iStride = sizeof(VTXMODEL); // 정점에 저장하기 위한 정보는 구조체 VTXMODEL 에
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXMODEL*			pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL));

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_AnimModel(aiMesh * pAIMesh)
{
	m_iStride = sizeof(VTXANIMMODEL); // 정점에 저장하기 위한 정보는 구조체 VTXANIMMODEL 에
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXANIMMODEL*			pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL));

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	// CMesh::Ready_VertexBuffer_AnimModel(
	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i]; // 진짜 Bones 정보를 가져왔다.

		_uint iNumWeights = pAIBone->mNumWeights; /* 이 뼈는 몇개의 정점에 영향을 주는가?! */

		for (_uint j = 0; j < iNumWeights; ++j)
		{
			_uint iVertexIndex = pAIBone->mWeights[j].mVertexId; // 첫 번째 영향을 주는 놈!

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.w)
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, aiMesh * pAIMesh)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMesh::Free()
{
	__super::Free();
}
