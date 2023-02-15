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

class CPlayer_Talk final : public CGameObject
{
public:
	typedef struct tagPlayerTalkInfo
	{
		enum TYPE { FINN, JAKE, TYPE_END};
		TYPE			eTextureType = TYPE_END;

		CTransform*		pTarget_TransformCom = { nullptr };

	}PLAYERTALKINFO;

private:
	CPlayer_Talk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Talk(const CPlayer_Talk& rhs);
	virtual ~CPlayer_Talk() = default;

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
	PLAYERTALKINFO			m_tPlayerTalkInfo;

	_int					m_iRandom_Texture = 0;
	_double					m_dDead_TimeAcc = 0.0;

	// Shader
	_float					m_fAlpha = 0.0f;

public:
	static	CPlayer_Talk*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END