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

class CE_Smoke final : public CGameObject
{
public:
	typedef struct tagSmoketInfo
	{
		_float3		f3Pos = { 0.0f, 0.0f, 0.0f };
		_float4		f4Look = { 0.0f, 0.0f, 0.0f, 0.0f };

	}SMOKEINFO;

private:
	CE_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Smoke(const CE_Smoke& rhs);
	virtual ~CE_Smoke() = default;

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
	SMOKEINFO			m_SmokeInfo;

private:	// Shader
	_float				m_fAlpha = 0.5f;
	_double				m_dEffect_TimeAcc = 0;

public:
	static	CE_Smoke*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END