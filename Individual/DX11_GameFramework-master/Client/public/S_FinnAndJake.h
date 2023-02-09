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
public:
	enum STATE { IDLE, MOVE, HIT, FINISH, STATE_END	};

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
	void					AnimatedMovie_Tick();
	
	void					KeyInput(const _double & TimeDelta);
	void					Hit_Tick(const _double & TimeDelta);

	void					Return_Tick();
	void					End_Tick();

private:
	_float4					m_f4NewLook = {0.0f, 0.0f, 0.0f, 1.0f};
	_float4					m_f4StartPoition = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	STATE					m_eAnim_State = STATE_END;
	_bool					m_OnMove = false;
	_bool					m_OnHit = false;
	_bool					m_bEnd = false;

	// ºŒ¿Ã¥ı
	_bool					m_bShader_Hit = false;
	_double					m_dShader_Hit_TimeAcc = 0;

public:
	static CS_FinnAndJake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END