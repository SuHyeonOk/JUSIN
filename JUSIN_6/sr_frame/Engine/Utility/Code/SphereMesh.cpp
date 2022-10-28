#include "..\..\Header\SphereMesh.h"

#define PI           3.14159265f


CSphereMesh::CSphereMesh(LPDIRECT3DDEVICE9 pGraphicDev) :CVIBuffer(pGraphicDev)
{
}

CSphereMesh::~CSphereMesh()
{
}

CSphereMesh::CSphereMesh(const CSphereMesh& rhs) : CVIBuffer()
{
	m_pGraphicDev = rhs.m_pGraphicDev;
	gpTextureMappingShader = rhs.gpTextureMappingShader;
	gpSphere = rhs.gpSphere;
	gpEarthDM = rhs.gpEarthDM;

	gpSphere->AddRef();
	gpTextureMappingShader->AddRef();
	gpEarthDM->AddRef();
	m_pGraphicDev->AddRef();
}



HRESULT CSphereMesh::Ready_Buffer(const wstring& _shaderfile, const wstring& _texturefile)
{
	return LoadAssets(_shaderfile, _texturefile);
}

void CSphereMesh::Render_Buffer()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);



	gpTextureMappingShader->SetMatrix("gWorldMatrix", &m_matWorld);

	_matrix view, proj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
	gpTextureMappingShader->SetMatrix("gViewMatrix", &view);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &proj);
	gpTextureMappingShader->SetMatrix("gProjectionMatrix", &proj);
	//~Matrix
	gpTextureMappingShader->SetTexture("DiffuseMap_Tex", gpEarthDM);

	UINT numPasses = 0;
	gpTextureMappingShader->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			gpTextureMappingShader->BeginPass(i);
			{
				// 구체를 그린다.
				gpSphere->DrawSubset(0);
			}
			gpTextureMappingShader->EndPass();
		}
	}
	gpTextureMappingShader->End();

	// CVIBuffer::Render_Buffer();
}

void CSphereMesh::Free()
{
	Safe_Release(gpSphere);
	Safe_Release(gpTextureMappingShader);
	Safe_Release(gpEarthDM);
	Safe_Release(m_pGraphicDev);
}

HRESULT CSphereMesh::LoadAssets(const wstring& _shaderfile, const wstring& _texturefile)
{
	gpEarthDM = LoadTexture(_texturefile.c_str());
	if (!gpEarthDM)
	{
		return false;
	}

	// 쉐이더 로딩
	gpTextureMappingShader = LoadShader(_shaderfile.c_str());
	if (!gpTextureMappingShader)
	{
		return false;
	}

	// 모델 로딩
	gpSphere = LoadModel(L"../Bin/Resource/Shader/Sphere.x");
	if (!gpSphere)
	{
		return false;
	}

	return S_OK;
}

LPDIRECT3DTEXTURE9 CSphereMesh::LoadTexture(LPCWSTR filename)
{
	LPDIRECT3DTEXTURE9 ret = NULL;
	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, filename, &ret)))
	{
		OutputDebugString(L"텍스처 로딩 실패: ");
		OutputDebugString(filename);
		OutputDebugString(L"\n");
	}

	return ret;
}

LPD3DXEFFECT CSphereMesh::LoadShader(LPCWSTR filename)
{
	LPD3DXEFFECT ret = NULL;

	LPD3DXBUFFER pError = NULL;
	DWORD dwShaderFlags = 0;

#if _DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	D3DXCreateEffectFromFile(m_pGraphicDev, filename,
		NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

	// 쉐이더 로딩에 실패한 경우 output창에 쉐이더
	// 컴파일 에러를 출력한다.
	if (!ret && pError)
	{
		int size = pError->GetBufferSize();
		void *ack = pError->GetBufferPointer();

		// if (ack)
		// {
		// 	char* str = new char[size];
		// 	sprintf(str, (const char*)ack, size);
		// 	OutputDebugString(LPCWSTR(str));
		// 	delete[] str;
		// }
	}

	return ret;
}

LPD3DXMESH CSphereMesh::LoadModel(LPCWSTR filename)
{
	LPD3DXMESH ret = NULL;
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, m_pGraphicDev, NULL, NULL, NULL, NULL, &ret)))
	{
		OutputDebugString(L"모델 로딩 실패: ");
		OutputDebugString(filename);
		OutputDebugString(L"\n");
	};
	//D3DXCreateSphere(m_pGraphicDev, 10.f, 10, 10, &ret, nullptr);

	return ret;
}

CSphereMesh* CSphereMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& _shaderfile,const wstring& _texturefile)
{
	CSphereMesh*	pInstance = new CSphereMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(_shaderfile, _texturefile)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}




	return pInstance;
}

CComponent* CSphereMesh::Clone()
{
	return new CSphereMesh(*this);
}
