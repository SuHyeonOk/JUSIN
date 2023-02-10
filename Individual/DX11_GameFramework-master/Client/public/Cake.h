#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CCake final : public CGameObject
{
public:
	enum TYPE { BLUE, MAGENTA, YELLOW, TYPE_END };

	typedef struct tagObjectInfo
	{
		TYPE			eType = TYPE_END;
		_float3			f3Position = { 0.0f, 0.0f, 0.0f };
	}OBJECTINFO;

private:
	CCake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCake(const CCake& rhs);
	virtual ~CCake() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

	virtual void	On_Collision(CGameObject* pOther) override;

private:
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	OBJECTINFO		m_ObjectInfo = {};

public:
	static	CCake*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END