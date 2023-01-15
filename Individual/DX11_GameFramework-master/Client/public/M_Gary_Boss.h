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

class CM_Gary_Boss final : public CGameObject
{
public:
	typedef struct tagGaryBossInfo
	{
		// 0 : ¿À¸¥¼Õ ÆòÅ¸
		// 1 : ¿Þ¼Õ ÆòÅ¸
		// 2 : µÚ·Î ³Ñ¾î°¥¶û DIE??
		// 3 : ¿ÞÂÊ Hit
		// 4 : ¼Õ Æî¶ô ¸»¶û
		// 5 : IDLE
		// 6 : Ãã
		// 7 : MOVE

		enum STATE { IDLE, MOVE, A_BULLET, A_STUN, A_CAGE, A_DANCE, HIT, DIE, STATE_END };

		STATE			eState = STATE_END;

		_float	fHP = 0.0f;
		_float	fMaxHP = 0.0f;
		_float	fAttack = 0.0f;
		_float	fExp = 0.0f;

	}BOSSINFO;

private:
	CM_Gary_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Gary_Boss(const CM_Gary_Boss& rhs);
	virtual ~CM_Gary_Boss() = default;

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
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

private:
	void				Monster_Tick(const _double& TimeDelta);
	void				Anim_Change(const _double& TimeDelta);

	void				Idle_Tick(const _double& TimeDelta);
	void				Move_Tick(const _double& TimeDelta);
	HRESULT				A_Bullet_Tick(const _double& TimeDelta);
	HRESULT				A_Stun_Tick(const _double& TimeDelta);
	HRESULT				A_Cage_Tick(const _double& TimeDelta);
	HRESULT				A_Dance_Tick(const _double& TimeDelta);
	void				Hit_Tick(const _double& TimeDelta);
	void				Die_Tick(const _double& TimeDelta);

private:
	BOSSINFO			m_tBossInfo;

	_bool				m_bShader_Hit = false;

public:
	static	CM_Gary_Boss*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END