#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CTexture : public CComponent
{
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() override;

public:
	public:
	HRESULT		Ready_Texture(const _tchar* pPath, TEXTUREID eType, const _uint& iCnt);
	void		Set_Texture(const _uint& iIndex = 0);		// 텍스처 그리기 관련 함수
	size_t		GetTexSize() const { return m_vecTexture.size(); }
	IDirect3DBaseTexture9* GetDXTexture(const _uint& iIndex = 0) { return m_vecTexture[iIndex]; }
	void		Set_Texture(class CShader* pShaderCom, D3DXHANDLE hHandle, const _uint& iIndex = 0);		// 텍스처 그리기 관련 함수

private:
	vector<IDirect3DBaseTexture9*>		m_vecTexture;

public:
	static	CTexture*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath, TEXTUREID eType, const _uint& iCnt = 1);
	virtual CComponent* Clone() override;
	virtual void Free() override;
};

END