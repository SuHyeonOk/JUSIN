#include "..\..\Header\Shader.h"

#include "Export_Function.h"

USING(Engine)

CShader::CShader(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

Engine::CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
{
	m_pEffect->AddRef();
}

HRESULT CShader::Ready_Shader(const _tchar * pShaderFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, pShaderFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL; // 여기까지 이첵트 객체를 만들기 완성

	// m_pEffect->SetMatrix("g_WorldMatrix", )
	/*m_pEffect->SetVector();
	m_pEffect->SetTexture();

	m_pEffect->SetFloat();*/

		

	return S_OK;
}

HRESULT CShader::Begin_Shader(_uint iPassIndex)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->Begin(nullptr, 0);
	m_pEffect->BeginPass(iPassIndex);

	return S_OK;
}

HRESULT CShader::End_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->End();

	return S_OK;
}


CShader * CShader::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar * pShaderFilePath)
{
	CShader* pInstance = new CShader(pDevice);

	if (FAILED(pInstance->Ready_Shader(pShaderFilePath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* Engine::CShader::Clone(void)
{
	return new CShader(*this);		
}

void CShader::Free(void)
{
	CComponent::Free();

	Safe_Release(m_pEffect);
}
