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

class CUI_3DTexture final : public CGameObject
{
public:
	typedef struct tag3DTextureInfo : public CGameObject::GAMEOBJECTDESC
	{
		enum TEXTTYPE { TYPE_FIND, TYPE_END };

		TEXTTYPE	eTextureType;

		_float3		f3Pos;
		_float2		f2Size;

	}TEXTUREINFO;

private:
	CUI_3DTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_3DTexture(const CUI_3DTexture& rhs);
	virtual ~CUI_3DTexture() = default;

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
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	
private:
	void					Find_Tick(const _double & TimeDelta);

private:
	TEXTUREINFO				m_tTextureInfo;

	_double					m_dTexture_TimeAcc = 0;	// Find_Tick();

public:
	static	CUI_3DTexture*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END