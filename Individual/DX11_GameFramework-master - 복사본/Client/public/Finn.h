#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Obj_Manager.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
END

BEGIN(Client)

class CFinn final : public CGameObject
{
private:
	CFinn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFinn(const CFinn& rhs);
	virtual ~CFinn() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual HRESULT		Render() override;
	virtual	HRESULT		Render_XRay() override;
	virtual HRESULT		Render_ShadowDepth() override;

	virtual void		On_Collision(CGameObject* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CShader*				m_pShaderXRayCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };
	CNavigation*			m_pNavigationCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShaderXRayResources();

	HRESULT Ready_Parts();	// 나 에서 생성할 객체들

private:
	void	Parts_Tick(const _double & TimeDelta);
	void	Parts_LateTick(const _double & TimeDelta);
	
private:
	void	Player_Tick(_double TimeDelta);
	void	Player_Skill_Tick(_double TimeDelta);
	void	Current_Player(_double TimeDelta);
	void	Player_Follow(_double TimeDelta);
	void	Check_Follow(_double TimeDelta);
	void	Key_Input(_double TimeDelta);

	void	Space_Attack_Tick(_double TimeDelta);
	void	Attack_Paint_Tick(_double TimeDelta);
	void	Skill_Marceline_Tick(_double TimeDelta);
	void	Skill_Coin_Tick(_double TimeDelta);
	void	Skill_Food_Tick(_double TimeDelta);
	HRESULT	Skill_Fiona_Tick(_double TimeDelta);

	void	Roolling_Tick(_double TimeDelta);
	void	Hit_Tick(_double TimeDelta);
	void	KnockBack_Hit_Tick(_double TimeDelta);
	void	Stun_Tick();
	void	Swim_Tick(_double TimeDelta);
	void	Change_Tick();
	void	Cheering_Tick();

	void	BossCage(const _double & TimeDelta);

	void	Current_HP(const _double & TimeDelta);
	void	Anim_Change(_double TimeDelta);

	HRESULT	Talk(const _double & TimeDelta);
	void	Sound_Tick();

private:
	vector<CGameObject*>		m_PlayerParts;
	CObj_Manager::PLAYERINFO	m_tPlayerInfo;

	_bool		m_OnMove = false;			// Key_Input() : 키를 누르면 treu 됨
	_float		m_fAngle = 0.0f;
	_float4		m_f4NewLook = { 0.0f, 0.0f, 0.0f, 0.0f };

	//////////////////////////// 플레이어 일 때

	_double		m_dKnockBack_Hit_TimeAcc = 0.0;

	_bool		m_bStun = false;			// 스턴
	_uint		m_iStun_Count = 0;			// 스턴 애니메이션 두 번

	_bool		m_bDiving = false;			// 수영
	_bool		m_bIsSwim = false;

	// Skill
	_bool		m_bOneSkillStart = false;
	_bool		m_bSkillStart = false;		// 스킬이 시작 되었다.
	_double		m_dSkill_TimeAcc = 0.0;

	_bool		m_bSkill_Clone = false;		// 스킬을 한 번만 생성하기 위해서

	//////////////////////////// 플레이어가 아닐 때

	_double		m_dNotfollow_TimeAcc = 0.0;	// Check_Follow() : Player 를 따라가지 못 하는 시간

	//////////////////////////// 계속

	_double		m_dBossCage_TimeAcc = 0.0;

	// 셰이더
	_float		m_fAlpha = 1.f;
	_bool		m_bShader_Hit = false;
	_double		m_dShader_Hit_TimeAcc = 0.0;

	// 이펙트
	_double		m_dEffect_Swim_TimeAcc = 0.0;

	// 대화
	_double		m_dTalk_TimeAcc = 0.0;

	// 소리
	_bool		m_bAttack_Sound = false;

public:
	static CFinn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END