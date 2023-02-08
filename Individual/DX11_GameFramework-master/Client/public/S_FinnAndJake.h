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

class CS_FinnAndJake final : public CGameObject
{
private:
	CS_FinnAndJake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_FinnAndJake(const CS_FinnAndJake& rhs);
	virtual ~CS_FinnAndJake() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_double TimeDelta) override;
	virtual void			Late_Tick(_double TimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	
private:
	void					KeyInput(const _double & TimeDelta);

private:
	_float3					m_f3Pos = _float3(0.f, 0.f, 0.f);
	_float4					m_f4NewLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	_bool					m_OnMove = false;

	// ºŒ¿Ã¥ı
	_bool					m_bShader_Hit = false;
	_double					m_dShader_Hit_TimeAcc = 0;

public:
	static CS_FinnAndJake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END