#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CFamilySword final : public CGameObject
{
private:
	CFamilySword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFamilySword(const CFamilySword& rhs);
	virtual ~CFamilySword() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_float m_fX = 0.0f;

public:
	static	CFamilySword*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END