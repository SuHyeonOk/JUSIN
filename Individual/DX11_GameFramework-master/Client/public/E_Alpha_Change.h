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

class CE_Alpha_Change final : public CGameObject
{
public:
	typedef struct tagEffectlInfo
	{
		enum TEXTURETYPE { HIT_TEXTURE, COLOR_HIT_TEXTURE, 
			JAKESON_TEXTURE, JAKESON_TRANSFORM_TEXTURE,
			BOOM_FIRE_TEXTURE, BURN_FIRE_TEXTURE, TEXTURE_END };
		TEXTURETYPE		eTextureType = TEXTURE_END;

		_float3			f3Pos = { 0.0f, 0.0f, 0.0f };
		_float3			f3Color = { 0.0f, 0.0f, 0.0f };

	}EFFECTINFO;

private:
	CE_Alpha_Change(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Alpha_Change(const CE_Alpha_Change& rhs);
	virtual ~CE_Alpha_Change() = default;

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
	EFFECTINFO				m_tEffectInfo;

private:	// Shader
	_int					m_iTexture_Index = 0;
	_double					m_dChange_Texture = 0;

	_int					m_iDead_Number = 0;
	_int					m_iDead_Count = 0;

public:
	static	CE_Alpha_Change*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END