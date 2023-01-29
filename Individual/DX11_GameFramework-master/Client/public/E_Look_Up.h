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

class CE_Look_Up final : public CGameObject
{
public:
	typedef struct tagEffectlInfo
	{
		enum TEXTURETYPE { SKELETON_TEXTURE, BOSS_SOMKE_TEXTURE, HP_TEXTURE, MINUSHP_TEXTURE, TEXTURE_END };
		TEXTURETYPE		eTextureType = TEXTURE_END;

		_float3			f3Pos = { 0.0f, 0.0f, 0.0f };
		_float3			f3Color = { 0.0f, 0.0f, 0.0f };

	}EFFECTINFO;

private:
	CE_Look_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Look_Up(const CE_Look_Up& rhs);
	virtual ~CE_Look_Up() = default;

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
	_float				m_fAlpha = 0.0f;

public:
	static	CE_Look_Up*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END