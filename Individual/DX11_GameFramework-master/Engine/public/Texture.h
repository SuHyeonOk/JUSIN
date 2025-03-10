#pragma once

#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
protected:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResources(class CShader* pShaderCom, const char* pConstantName); // 텍스처 여러장 받아오는 함수
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);

private:
	_uint										m_iNumTextures = 0;
	ID3D11ShaderResourceView**					m_pTextures = nullptr;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END