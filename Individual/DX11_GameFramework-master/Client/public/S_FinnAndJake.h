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
	enum STATE { IDLE, MOVE, HIT, FINISH, STATE_END };

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
	virtual	HRESULT			Render_XRay() override;

	virtual void			On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CShader*				m_pShaderXRayCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CTransform*				m_pPlayer_TransformCom = { nullptr };

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	void					AnimatedMovie_Tick();

	void					KeyInput(const _double & TimeDelta);
	void					Hit_Tick(const _double & TimeDelta);
	void					KnockBack_Tick(const _double & TimeDelta);

	void					Return_Tick(const _double & TimeDelta);

	void					Rainicorn(const _double & TimeDelta);

	void					Shader_Alpha(const _double & TimeDelta);

private:
	_float4					m_f4NewLook = { 0.0f, 0.0f, 0.0f, 1.0f };
	_float4					m_f4StartPoition = { 0.0f, 0.0f, 0.0f, 1.0f };

	STATE					m_eAnim_State = { STATE_END };
	_bool					m_bOnMove = { false };
	_bool					m_bOnHit = { false };

	_bool					m_bRainicorn = { false };
	_double					m_dRainicorn_TimeAcc = { 0.0 };

	_bool					m_OnKnockBack = { false };
	_double					m_dKnockBack_TimeAcc = { 0.0 };

	// ºŒ¿Ã¥ı
	_bool					m_bShader_Hit = { false };
	_double					m_dShader_Hit_TimeAcc = { 0 };
	_bool					m_bAlpha_Down = { false };
	_bool					m_bAlpha_Up = { false };
	_bool					m_bAlpha_Change = { false };
	_float					m_fAlpha = { 1.f };

	// ¿Ã∆Â∆Æ
	_double					m_dEffect_TimeAcc = { 0.0 };

	_int	m_fX = 0;
	_int	m_fY = 0;

public:
	static CS_FinnAndJake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END