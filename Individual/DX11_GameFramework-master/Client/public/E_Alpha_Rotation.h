#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CE_Alpha_Rotation final : public CGameObject
{
public:
	typedef struct tagEffectlInfo
	{
		enum TEXTURETYPE { SHIELD_TEXTURE, POTAL_STAR_TEXTURE, TEXTURETYPE_END };
		TEXTURETYPE	eTextureType = TEXTURETYPE_END;

		_float3		f3Pos = { 0.0f, 0.0f, 0.0f };
		_float3		f3Color = { 0.0f, 0.0f, 0.0f };

	}EFFECTINFO;

private:
	CE_Alpha_Rotation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Alpha_Rotation(const CE_Alpha_Rotation& rhs);
	virtual ~CE_Alpha_Rotation() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	EFFECTINFO			m_tEffectInfo;

private:	// Shader
	_float				m_fAlpha = 1.0f;
	_float				m_fSizeX = 0.0f;
	_float				m_fSizeY = 0.0f;

public:
	static	CE_Alpha_Rotation*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END