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

class CBoss_Fan final : public CGameObject
{
	enum STATE { DANCE, HIT, DIE, STATE_END };

private:
	CBoss_Fan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Fan(const CBoss_Fan& rhs);
	virtual ~CBoss_Fan() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	void			Monster_Tick(const _double& TimeDelta);

	void			Dance_Tick(const _double& TimeDelta);
	void			Hit_Tick(const _double& TimeDelta);
	void			Die(const _double& TimeDelta);

private:
	STATE			m_eState = STATE_END;
	_float			m_fHP = 0.0f;
	_float			m_fMaxHP = 0.0f;

	_bool			m_bShader_Hit = false;
	_double			m_dShader_Hit_TimeAcc = 0;
	
	_float			m_fAlpha = 1.0f;

	_int			m_iDieEffect_Count = 0;

	_double			m_dEffect_TimeAcc = { 0.0 };

public:
	static	CBoss_Fan*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END