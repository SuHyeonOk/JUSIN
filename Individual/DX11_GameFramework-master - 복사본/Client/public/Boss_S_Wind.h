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

class CBoss_S_Wind final : public CGameObject
{
public:
	typedef struct tagBossSkillInfo
	{
		_float3		f3Pos = { 0.0f, 0.0f, 0.0f };
		_float4		f4Look = { 0.0f, 0.0f, 0.0f, 0.0f };

	}BOSSSKILLINFO;

private:
	CBoss_S_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_S_Wind(const CBoss_S_Wind& rhs);
	virtual ~CBoss_S_Wind() = default;

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
	BOSSSKILLINFO		m_tSkillInfo;

	_float				m_fAlpha = 1.0f;
	_float				m_fSizeX = 0.0f;
	_float				m_fSizeY = 0.0f;


	_float	m_fX = 0.0f;
	_float	m_fY = 0.0f;
	_float	m_fZ = 0.0f;

public:
	static	CBoss_S_Wind*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END