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

class CE_Beneficial final : public CGameObject
{
public:
	typedef struct tagBeneficialInfo
	{
		_float3		f3Pos = { 0.0f, 0.0f, 0.0f };
		_float3		f3Color = { 1.0f, 1.0f, 1.0f };

	}BENEFICIALINFO;

private:
	CE_Beneficial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Beneficial(const CE_Beneficial& rhs);
	virtual ~CE_Beneficial() = default;

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
	BENEFICIALINFO			m_tBeneficialInfo;

private:	// Shader
	_int					m_iTexture_Index = 0;
	_double					m_dChange_Texture = 0;
	_float					m_fAlpha = 0.7f;

public:
	static	CE_Beneficial*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END