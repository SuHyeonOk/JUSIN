#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CS_Jake_Son_Transform final : public CGameObject
{
	enum STATE {IDLE, RUN, ATTACK, SKILL, STATE_END };

private:
	CS_Jake_Son_Transform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CS_Jake_Son_Transform(const CS_Jake_Son_Transform& rhs);
	virtual ~CS_Jake_Son_Transform() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override {};

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	CTransform*				m_pJake_TransformCom = nullptr;
	CNavigation*			m_pJake_NavigationCom = nullptr;

	CTransform*				m_pBoss_TransformCom = nullptr;

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

private:
	void				JakeSon_Tick(const _double & TimeDelta);
	void				State_Tick(const _double & TimeDelta);

	void				Player_Follow(const _double & TimeDelta);
	void				Attack_Tick(const _double & TimeDelta);
	HRESULT				Skill_Tick(const _double & TimeDelta);

private:
	_float3				m_f3Pos = _float3(0.f, 0.f, 0.f);
	
	STATE				m_eState = STATE_END;

	_double				m_dTwister_TimeAcc = { 0.0 };



public:
	static CS_Jake_Son_Transform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END