#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CO_Collider final : public CGameObject
{
public:
	typedef struct tagColliderInfo
	{
		enum TYPE		{ CUTSCENE_ONE, TYPE_END };
		TYPE			eType = TYPE_END;

		_float3			f3Pos = { 0.0f, 0.0f, 0.0f };

	}COLLIDERINFO;

private:
	CO_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CO_Collider(const CO_Collider& rhs);
	virtual ~CO_Collider() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

private:
	COLLIDERINFO		m_ColliderInfo = {};

public:
	static	CO_Collider*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END