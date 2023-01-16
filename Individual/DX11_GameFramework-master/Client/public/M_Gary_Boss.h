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
	enum STATE { IDLE, A_MOVE, A_BULLET, A_STUN, A_CAGE, A_DANCE, HIT, DIE, STATE_END };

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
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();

private:
	void				Monster_Tick(const _double& TimeDelta);
	void				Anim_Change();

	// TODO : 구조 다 짜고, TimeDelta 필요없는 함수는 삭제하기
	void				Idle_Tick(const _double& TimeDelta);
	void				RandomMove(const _double& TimeDelta);
	void				A_Move_Tick(const _double& TimeDelta);
	HRESULT				A_Bullet_Tick(const _double& TimeDelta);
	HRESULT				A_Stun_Tick(const _double& TimeDelta);
	HRESULT				A_Cage_Tick(const _double& TimeDelta);
	void				A_Dance_Tick(const _double& TimeDelta);
	HRESULT				Fann_Create();
	_bool				Fann_Dead_Check();
	void				Hit_Tick(const _double& TimeDelta);
	void				Die_Tick(const _double& TimeDelta);

private:
	// 기본 데이터
	STATE				m_eState = STATE_END;
	STATE				m_eAnimState = STATE_END;
	_float				m_fHP		= 0.0f;
	_float				m_fMaxHP	= 0.0f;
	_float				m_fAttack	= 0.0f;
	_float				m_fExp		= 0.0f;

	_float4				m_f4CenterPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	// 여러 곳 에서 활용 변수
	_bool				m_bSkill = false;
	_double				m_dSkill_TimeAcc = 0;
	
	// A_Bullet_Tick()
	_int				m_iBullet_Count = 0;

	// A_Stun_Tick()
	_bool				m_bEffect_Smoke = false;

	// A_Move_Tick()
	_float4				m_f4PlayerPos = { 0.0f, 0.0f, 0.0f, 1.0f };


	// 셰이더
	_bool				m_bShader_Hit = false;
	_double				m_dShader_Hit_TimeAcc = 0;

public:
	static	CM_Gary_Boss*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END