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

class CS_Jake_Son_Twister final : public CGameObject
{
private:
	CS_Jake_Son_Twister(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Jake_Son_Twister(const CS_Jake_Son_Twister& rhs);
	virtual ~CS_Jake_Son_Twister() = default;

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
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	CTransform*			m_pJakeSon_TransformCom = { nullptr };
	CTransform*			m_pBoss_TransformCom = { nullptr };

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

public:
	static	CS_Jake_Son_Twister*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr) override;
	virtual void						Free() override;
};

END