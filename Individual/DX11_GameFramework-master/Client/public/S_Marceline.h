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

class CS_Marceline final : public CGameObject
{
private:
	CS_Marceline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Marceline(const CS_Marceline& rhs);
	virtual ~CS_Marceline() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	_bool			m_Player_Strun = false;

public:
	static	CS_Marceline* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free() override;
};

END