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
	enum STATE { IDLE, MOVE, A_MOVE, A_ATTACK, A_BULLET, A_STUN, A_CAGE, A_DANCE, HIT, DIE, STATE_END };

private:
	CM_Gary_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Gary_Boss(const CM_Gary_Boss& rhs);
	virtual ~CM_Gary_Boss() = default;

public:
	void				Set_CutScene() { m_bCutScene = true; }

	_float				Get_BossHp() {
		return m_fHP;
	}
	_float				Get_BossMaxHp() {
		return m_fMaxHP;
	}

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

	void				Idle_Tick(const _double& TimeDelta);
	void				Random_Skill(const _double& TimeDelta);
	void				RandomMove(const _double& TimeDelta);

	HRESULT				A_Move_Tick(const _double& TimeDelta);
	HRESULT				A_Bullet_Tick(const _double& TimeDelta);
	HRESULT				A_Stun_Tick(const _double& TimeDelta);
	HRESULT				A_Cage_Tick(const _double& TimeDelta);
	void				A_Dance_Tick(const _double& TimeDelta);
	HRESULT				Fann_Create();
	_bool				Fann_Dead_Check();

	void				Hit_Tick(const _double& TimeDelta);
	void				Die_Tick(const _double& TimeDelta);
	void				LookChange(const _double& TimeDelta);

	void				Effect_Tick(const _double& TimeDelta);
	void				Shader_Alpha(const _double & TimeDelta);

	void				CutScene_Tick(const _double & TimeDelta);

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
	_bool				m_bMovePos = false;
	_bool				m_bSkill = false;
	_double				m_dSkill_TimeAcc = 0;

	_int				m_iBullet_Count = 0;

	// A_Move_Tick()
	_bool				m_bAttack_Animation = false;

	// A_Move_Tick()
	_float4				m_f4PlayerPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Move()
	_bool				m_bMove = false;
	_float4				m_f4MovemPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Random_Skill()
	_int				m_iSkill_Data = 0;

	// Hit()
	_bool				m_bHit = false;

	_bool				m_bLookChange = false;
	_double				m_dLookChange_TimeAcc = 0.0;
	
	// 이펙트
	_bool				m_bEffect = false;
	_int				m_iEffect_Count = 0;
	_double				m_dEffect_TimeAcc = 0;

	// 셰이더
	_bool				m_bShader_Alpha = false;
	_float				m_fAlpha = 1.f;
	_bool				m_bShader_Hit = false;
	_double				m_dShader_Hit_TimeAcc = 0;

	// CutScene
	_bool				m_bCutScene = false;
	_bool				m_bCutSceneEnd = false;

public:
	static	CM_Gary_Boss*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END