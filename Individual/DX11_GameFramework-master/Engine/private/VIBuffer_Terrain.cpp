#include "..\public\VIBuffer_Terrain.h"

#include "Picking.h"
#include "Transform.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
	
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{

}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER			fh;
	BITMAPINFOHEADER			ih;
	_ulong*						pPixel = nullptr;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;	
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	pPixel = new _ulong[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;	
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	m_pVerticesPos = new _float3[m_iNumVertices];

	VTXNORTEX*			pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX));

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

		//	11111111 11111011 11111011 11111011
		//& 00000000 00000000 00000000 11111111

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(_float(j), (pPixel[iIndex] & 0x000000ff) / 10.f, _float(i));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(_float(j) / (m_iNumVerticesX - 1.0f), _float(i) / (m_iNumVerticesZ - 1.0f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion



#pragma region INDEX_BUFFER

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint			iIndex = i * m_iNumVerticesX + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1, 
				iIndex + 1, 
				iIndex
			};

			_vector			vSour, vDest, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;
		}
	}

#pragma endregion


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CVIBuffer_Terrain::Picking(CTransform * pTransform, _float4* pOut)
{
	CPicking*		pPicking = CPicking::GetInstance();
	Safe_AddRef(pPicking);

	_matrix		WorldMatrixInv = pTransform->Get_WorldMatrix_Inverse();
	_float4		f4RayDir, f4RayPos;

	pPicking->Compute_LocalRayInfo(&f4RayDir, &f4RayPos, pTransform);

	_vector vRayPos, vRayDir;
	vRayPos = XMLoadFloat4(&f4RayPos);
	vRayDir = XMLoadFloat4(&f4RayDir);
	vRayDir = XMVector3Normalize(vRayDir);

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_float		fDist;
			_matrix		WoreldMatrix = pTransform->Get_WorldMatrix();

			_vector		VerticesPos_0, VerticesPos_1, VerticesPos_2, VerticesPos_3;
			VerticesPos_0 = XMLoadFloat3(&m_pVerticesPos[iIndices[0]]);
			VerticesPos_1 = XMLoadFloat3(&m_pVerticesPos[iIndices[1]]);
			VerticesPos_2 = XMLoadFloat3(&m_pVerticesPos[iIndices[2]]);
			VerticesPos_3 = XMLoadFloat3(&m_pVerticesPos[iIndices[3]]);

			// 오른쪽 상단
			if (true == TriangleTests::Intersects(vRayPos, vRayDir, VerticesPos_0, VerticesPos_1, VerticesPos_2, fDist))
			{
				_vector vPickPos = vRayPos + vRayDir * fDist;

				// TODO : 1) vector을 더 많이 사용 (인자값 _vector로 변경 해야함)
				//pOut = XMVector3TransformCoord(vPickPos, WoreldMatrix);

				// TODO : 2) _float4를 더 많이 사용
				_vector vOutTemp;
				vOutTemp = XMVector3TransformCoord(vPickPos, WoreldMatrix);
				XMStoreFloat4(pOut, vOutTemp); 

				Safe_Release(pPicking);
				return true;
			}
			
			// 왼쪽 하단 
			if (true == TriangleTests::Intersects(vRayPos, vRayDir, VerticesPos_0, VerticesPos_2, VerticesPos_3, fDist))
			{
				_vector vPickPos = vRayPos + vRayDir * fDist;

				_vector vOutTemp;
				vOutTemp = XMVector3TransformCoord(vPickPos, WoreldMatrix);
				XMStoreFloat4(pOut, vOutTemp);

				Safe_Release(pPicking);
				return true;
			}
		}
	}
	
	Safe_Release(pPicking);
	return false;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
