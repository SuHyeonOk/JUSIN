#include "..\public\Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_InputLayouts(rhs.m_InputLayouts)	
	, m_iNumPasses(rhs.m_iNumPasses)
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

}

HRESULT CShader::Initialize_Prototype(const _tchar * pShaderFilePath, 
	const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements)
{
	_uint			iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
	
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;	

	// CShader::Initialize_Prototype()
	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	// GetDesc() -> Technique의 정보를 얻어오는 함수
	pTechnique->GetDesc(&TechniqueDesc);

	// TechniqueDesc.Passes -> Technique안에 pass의 개수
	m_iNumPasses = TechniqueDesc.Passes;

	// CShader::Initialize_Prototype()
	for (_uint i = 0; i < m_iNumPasses; ++i)
	{	
		// pass의 개수 만큼 디바이스 객체를 통해서 InputLayout 을 만들어 주자.
		// m_pDevice->CreateInputLayout()
		ID3D11InputLayout*		pInputLayout = nullptr;
		
		// pTechnique을 통해 특정 인덱스에 해당하는 pass의 정보를 얻어온다.
		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);

		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);

		/* 1인자 pElements을 위한 구조체
		D3D11_INPUT_ELEMENT_DESC : 정점의 멤버변수 하나를 표현하기위한 데이터. */
		/*
		LPCSTR SemanticName;
		UINT SemanticIndex;
		DXGI_FORMAT Format;
		UINT InputSlot;
		UINT AlignedByteOffset;
		D3D11_INPUT_CLASSIFICATION InputSlotClass;
		UINT InstanceDataStepRate;
		*/	

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, 
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		// CShader::Initialize_Prototype()
		m_InputLayouts.push_back(pInputLayout);
	}	

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (nullptr == m_pContext || 
		iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass*	pPass = pTechnique->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	pPass->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

CShader * CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements)
{
	CShader*		pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
