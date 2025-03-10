#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CO_TextureObject final : public CGameObject
{
public:
	typedef struct tagTextureObjectInfo
	{
		enum TEXTURETYPE { PORTAL, MINIGAME, TEXTURETYPE_END };
		TEXTURETYPE		eTextureType;

		_float3			f3Pos = { 0.0f, 0.0f, 0.0f };

	}TEXTUREOBJECT;

	void	Set_Portal() {
		m_bPortal = true;
	}

private:
	CO_TextureObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CO_TextureObject(const CO_TextureObject& rhs);
	virtual ~CO_TextureObject() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	TEXTUREOBJECT			m_TextureObject;

	_float					m_fAlpha = 1.0f;
	_double					m_bEffect_TimeAcc = 0;

	_bool					m_bPortal = { false };

public:
	static	CO_TextureObject*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END