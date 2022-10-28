#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CShader :	public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pDevice);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT Ready_Shader(const _tchar* pShaderFilePath);

	HRESULT Begin_Shader(_uint iPassIndex);

	HRESULT End_Shader();
public:
	HRESULT Set_RawValue(D3DXHANDLE hHandle, const void* pData, _uint iLength);
	HRESULT Set_Texture(D3DXHANDLE hHandle, IDirect3DBaseTexture9* pTexture);
	HRESULT Set_Bool(D3DXHANDLE hHandle, bool pData)
	{
		if (m_pEffect == nullptr) return E_FAIL;

		return m_pEffect->SetBool(hHandle, pData);
	}

private:
	LPD3DXEFFECT				m_pEffect = nullptr;

public:
	static CShader*		Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pShaderFilePath);
	virtual CComponent*	Clone(void);
private:
	virtual void Free(void);
};

END