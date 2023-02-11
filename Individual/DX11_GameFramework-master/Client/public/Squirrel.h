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

class CSquirrel final : public CGameObject
{
public:
	enum STATE { IDLE, ATTACK, MOVE, DANCE, EAT, STATE_END };

private:
	CSquirrel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSquirrel(const CSquirrel& rhs);
	virtual ~CSquirrel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

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
	void			AnimatedMovie_Tick();
	void			State_Tick(const _double & TimeDelta);

private:
	void			Idle_Tick();
	void			Move_Tick(const _double & TimeDelta);
	void			Attack_Tick(const _double & TimeDelta);
	void			Eat_Tick(const _double & TimeDelta);

private:
	STATE			m_eAnimState = STATE_END;
	STATE			m_eState = STATE_END;

	_float4			m_f4Start_Position = { 0.0f, 0.0f, 0.0f, 1.0f };

public:
	static	CSquirrel*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END