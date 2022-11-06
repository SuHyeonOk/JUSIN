#pragma once
#include "Component.h"

enum DamageType
{
	DT_STUN,
	DT_KNOCK_BACK,
	DT_STIFFEN,
	DT_HUGE_KNOCK_BACK,
	DT_HIGH_KNOCK_BACK,
	DT_KOUKU_SYMBOL_BLUE,
	DT_KOUKU_SYMBOL_RED,
	DT_SATON_FASCINATED,
	DT_SATON_GRAPED,
	DT_JUMP,
	// posion .. etc
	DT_END
};

class CTerrainCubeMap;
class CStun;
class CFascinate_Effect;

class CStatComponent : public CComponent
{
protected:
	explicit CStatComponent();
	explicit CStatComponent(const CStatComponent& rhs);
	virtual ~CStatComponent() override;

public:
	virtual _int Update_Component(const _float& fTimeDelta) override;
	virtual CComponent* Clone() override;
	virtual void Free() override;
	static CStatComponent* Create();

	void SetOwner(CGameObject* pOwner) {NULL_CHECK(pOwner); m_pOwner = pOwner;}
	void SetTransform(Engine::CTransform* pOwnerTrans) { NULL_CHECK(pOwnerTrans); m_pOwnerTrans = pOwnerTrans; m_pOwnerTrans->AddRef(); };
	void SetMaxHP(_uint iMaxHP) { m_iMaxHP = iMaxHP; m_iHP = static_cast<_int>(m_iMaxHP); }

	_int GetHP() const { return m_iHP; }
	_uint GetMaxHP() const { return m_iMaxHP;}

	void ModifyHP(_int iModifyingHP, _bool bEffect = true);
	void TakeDamage(_int iDamage, _vec3 vFromPos, CGameObject* pCauser, DamageType eType = DT_END, _bool bCritical = false);
	void SetJump(_bool bJump) { m_bJump = bJump; }
	void SetDead();

	_bool IsStun() const { return m_bStun || m_bKnockback; }
	_bool IsDead() const { return m_bDead; }
	_bool IsDamaged() const { return m_bDamaged; }
	_bool IsSatonSymbol_Blue() const { return m_bSatonSymbol_Blue; }
	_bool IsSatonSymbol_Red() const { return m_bSatonSymbol_Red; }
	_bool IsSatonFascinate() const { return m_bFascinated; }

	void IsSaton() { m_bIsSaton = !m_bIsSaton; }
	void Graped_Off() { m_bGraped = false; }
	void Revive();
	void SetHP(_int iHP) { m_iHP = iHP;}

	// cur hp, max hp, damage
	BaseDelegater<_int, _uint, _int> m_DamageDelegater;
	void SetHurtSound(const vector<wstring>& vHurtSound)	{ m_vHurtSound = vHurtSound;}

private:
	Engine::CTransform* m_pOwnerTrans = nullptr;
	CTerrainCubeMap* m_pCubeMap = nullptr;
	CGameObject* m_pOwner = nullptr;

	vector<wstring> m_vHurtSound;

	_uint m_iMaxHP = 50;
	_int m_iHP = 50;
	_bool m_bDead = false;

	// stun
	_bool m_bStun = false;
	_float m_fStunTime = 2.f;
	_float m_fCurStunTime = 2.f;
	CStun* m_pStun = nullptr;

	// knock back
	_bool m_bKnockback = false;
	_float m_fKnockbackTime = 2.f;
	_float m_fCurKnockbackTime = 2.f;
	_float m_fPreYPos = 0.f;

	_bool m_bStiffen = false;
	_float m_fStiffeTime = 10.2f;
	_float m_fCurStiffeTime = 0.1f;

	_vec3 m_vKnockBackVelocity = CGameUtilMgr::s_vZero;

	// damaged
	_bool m_bDamaged = false;
	_float m_fDamagedTime = 0.2f;
	_float m_fCurDamagedTime = 0.2f;

	// Saton Symbol
	_bool m_bSatonSymbol_Blue = false;
	_bool m_bSatonSymbol_Red = false;

	_float m_fSatonSymbolTime = 0.3f;
	_float m_fCurSatonSymbolTime = 0.3f;

	// Saton fascinate
	_bool m_bFascinated = false;
	_float m_fSatonFascinatedTime = 4.f;
	_float m_fCurSatonFascinatedTime = 0.f;
	class CFascinate* m_pFaci = nullptr;
	// CFascinate_Effect* m_pFascinate = nullptr;

	// Saton Grap
	_bool m_bGraped = false;
	_float m_fHighKnockBackTime = 2.f;
	_float m_fCurHighKnockBackTime = 2.f;

	// ~Saton Grap

	_bool m_bIsSaton = false;

	//jump
	_bool m_bJump = false;
	_float m_fJumpTime = 3.f;
	_float m_fCurJumpTime = 1.f;

};

