#pragma once
#include "Monster.h"
#include "Particle.h"

class CController;
class CPlayer;
class CSaton :	public CMonster
{
private:
	enum CStatonState
	{
		INTRO,
		IDLE,
		FIRSTATTACK,
		SATON_BIRD,
		SATON_GRAP,
		SATON_SYMBOL,
		SATON_FASCINATE,
		DEAD,
		STATE_END
	};

private:
	explicit CSaton(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSaton(const CMonster& rhs);
	virtual ~CSaton()override;

public:
	virtual HRESULT Ready_Object() override;
	virtual void AnimationEvent(const string& strEvent) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object() override;
	virtual void Free() override;
	void IsDeadTrue() { m_bDelete = true; }
	static CSaton* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote = false);

	virtual void StateChange();

	void FirstAttack(const _vec3& vTargetPos)
	{
		m_bFirstHammerAttack = true;
		m_vTargetPos = vTargetPos;
	}
	
	void SatonGrap(const _vec3& vTargetPos)
	{
		m_bSatonGrap = true;
		m_vTargetPos = vTargetPos;
	}

	void SatonBird(const _vec3& vTargetPos)
	{
		m_bSatonBird = true;
		m_vTargetPos = vTargetPos;
	}

	void SatonSymbolAnim(const _vec3& vTargetPos)
	{
		m_bSatonSymbolAnim = true;
		m_vTargetPos = vTargetPos;
	}

	void SatonFascinate(const _vec3& vBossTargetPos, const _vec3& vMoonTargetPos)
	{
		m_bSatonFascinate = true;
		m_vTargetPos = vBossTargetPos;
		Engine::Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(m_vTargetPos, 1.f, D3DXCOLOR(1.f,1.f,1.f,1.f), 1, 3.f, 0);
	}

	void SatonDrawMoon(const _vec3& vMoonTargetPos)
	{
		m_vTargetPos = vMoonTargetPos;
		m_vExplodMoonPos = vMoonTargetPos;
		m_vTargetPos.y += 1.f;
		m_vExplodMoonPos.y += 1.f;
		Engine::Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(m_vTargetPos, 1.f, RED, 1, 2.f, 0);
		m_bSatonDrawMoon = true;
	}

	void SatonShockPowder(void);


	// controller 조종 함수
	_vec3 Get_TargetPos() { return m_vTargetPos; }
	void WalkToTarget(const _vec3& vTargetPos) { m_vTargetPos = vTargetPos; }

	//m_bMove = true;
private:
	CStatonState m_eState = STATE_END;
	array<CubeAnimFrame, STATE_END> m_arrAnim;

	void	SetOff()
	{
		m_bFirstHammerAttack = false;
		m_bSatonGrap = false;
		m_bSatonBird = false;
		m_bSatonSymbolAnim = false;
		m_bSatonFascinate = false;
		m_bSatonDrawMoon = false;
	}

	// true : PlayAnimationOnce 사용 가능 상태(동작 애니메이션 실행 가능), false: 다른 애니메이션 실행중
	_bool m_bCanPlayAnim = true; // 현재 실행중인 애니메이션 끊고 애니메이션 실행 가능 여부

	_vec3 m_vTargetPos = CGameUtilMgr::s_vZero; // controller 입력

	_bool m_bFirstHammerAttack = false; // controller 입력

	//SatonGrab
	_bool m_bSatonGrap = false;
	_bool m_bIsGrap = false;
	_bool m_bIsGrapEndAttack = false;
	// ~SatonGrab

	//SatoCat
	_bool m_bSatonBird = false;
	//~SatoCat

	//SatonSymbol
	_bool m_bSatonSymbolAnim = false;
	//~SatonSymbol

	//SatonFascinate
	_bool m_bSatonFascinate = false;
	_bool m_bSatonDrawMoon = false;
	_bool m_bSatonDrawMoonPair = false;
	_bool m_bStatonExplodeMoon = false;
	_vec3 m_vExplodMoonPos;
	//~SatonFascinate

	//Coll
	_bool m_bIsAttack_1_Coll = false;
	_bool m_bIsAttack_2_Coll = false;
	_bool m_bIsCatColl = false;
	//~Coll

	//AttackRangeCircle
	_vec3 m_vATKRNGCirclePos;
	//AttackRangeCircle

	_float m_fTime;
	_float m_fCurTime;

	//intro
	_bool m_bSatonIntro = true;

	set<CPlayer*> m_pGrabbedList;
};

