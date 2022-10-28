#include "stdafx.h"
#include "../Header/StatComponent.h"
#include "AbstFactory.h"
#include "Particle.h"
#include "TerrainCubeMap.h"
#include "DamageFontMgr.h"
#include "Kouku.h"
#include "ServerPacketHandler.h"

CStatComponent::CStatComponent()
{
}

CStatComponent::CStatComponent(const CStatComponent& rhs): CComponent(rhs)
{
}

CStatComponent::~CStatComponent()
{
}

_int CStatComponent::Update_Component(const _float& fTimeDelta)
{
	if (m_bDead) return 0;

	if (m_pCubeMap == nullptr)
	{
		m_pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
		m_pCubeMap->AddRef();

		CCollisionCom* pColl = m_pOwner->Get_Component<CCollisionCom>(L"Proto_CollisionCom", ID_DYNAMIC);
		pColl->SetStaticCallBack([this]()
		{
			m_vKnockBackVelocity = CGameUtilMgr::s_vZero;
			m_bKnockback = false;
		});
	}

	if (m_bStun)
	{
		if (m_fStunTime < m_fCurStunTime)
		{
			m_bStun = false;
			if (m_pStun)
			{
				m_pStun->SetDead();
				m_pStun = nullptr;
			}
		}
		else
		{
			m_fCurStunTime += fTimeDelta;
			if (m_pStun)
				m_pStun->SetPos(m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{0.f, 3.f, 0.f});
		}
	}

	if (m_bKnockback)
	{
		if (m_fKnockbackTime < m_fCurKnockbackTime)
			m_bKnockback = false;
		else
			m_fCurKnockbackTime += fTimeDelta;
	}

	//if (m_bKnockback)
	//{
	//	if (m_fHighKnockBackTime < m_fCurHighKnockBackTime)
	//		m_bKnockback = false;
	//	else
	//		m_fCurHighKnockBackTime += fTimeDelta;
	//}

	if (m_bStiffen)
	{
		if (m_fStiffeTime < m_fCurStiffeTime)
			m_bStiffen = false;
		else
			m_fCurStiffeTime += fTimeDelta;
	}

	if (m_bDamaged)
	{
		if (m_fDamagedTime < m_fCurDamagedTime)
			m_bDamaged = false;
		else
			m_fCurDamagedTime += fTimeDelta;
	}

	if (m_bFascinated)
	{
		if (m_fSatonFascinatedTime < m_fCurSatonFascinatedTime)
		{
			m_bFascinated = false;
			if (m_pFaci)
			{
				m_pFaci->SetDead();
				m_pFaci = nullptr;
			}
		}
		else
		{
			m_fCurSatonFascinatedTime += fTimeDelta;
			if (m_pFaci)
				m_pFaci->SetPos(m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{0.f, 3.f, 0.f});
		}
	}

	if (m_bJump)
	{
		if (m_fJumpTime < m_fCurJumpTime)
		{
			m_bJump = false;
			m_bGraped = false;
		}
		else
		{
			m_fCurJumpTime += fTimeDelta;
		}
	}


	// 세이튼 장판 기믹 때만 해당되게 구현하기 
	if (m_bSatonSymbol_Blue)
	{
		if (m_fSatonSymbolTime < m_fCurSatonSymbolTime)
			m_bSatonSymbol_Blue = false;
		else
		{
			m_fCurSatonSymbolTime += fTimeDelta;
		}
	}

	// if(m_fCurSatonSymbolTime)
	// {
	// 	Engine::Get_GameObject<CFascinated_Effect>(LAYER_EFFECT, L"Fascinate_Effect")->Add_Particle(m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{ 0.f, 3.f, 0.f }, 1.f, RED, 1, 0.1f, 0);
	// }


	if (!m_bGraped)
	{
		_vec3& vPos = m_pOwnerTrans->m_vInfo[INFO_POS];
		if (CGameUtilMgr::Vec3Cmp(m_vKnockBackVelocity, CGameUtilMgr::s_vZero, 0.1f))
		{
			if (!m_bJump)
				vPos.y = m_pCubeMap->GetHeight(vPos.x, vPos.z);
		}
		else
		{
			// 넉백 상태

			vPos += m_vKnockBackVelocity * fTimeDelta;

			m_vKnockBackVelocity.y -= 80.f * fTimeDelta;

			if (vPos.y <= m_pCubeMap->GetHeight(vPos.x, vPos.z) || m_bKnockback == false)
			{
				m_vKnockBackVelocity = CGameUtilMgr::s_vZero;
				m_bKnockback = false;
			}
		}
	}


	return 0;
}

CComponent* CStatComponent::Clone()
{
	return new CStatComponent(*this);
}

void CStatComponent::Free()
{
	CComponent::Free();
	Safe_Release(m_pOwnerTrans);
	Safe_Release(m_pCubeMap);
}

CStatComponent* CStatComponent::Create()
{
	return new CStatComponent();
}

void CStatComponent::ModifyHP(_int iModifyingHP, _bool bEffect)
{
	m_iHP += iModifyingHP;

	if (iModifyingHP < 0)
	{
		m_bDamaged = true;
		m_fCurDamagedTime = 0.f;

		if (bEffect)
		{
			// 피타격 이펙트
			Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")
				->Add_Particle(m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{0.f, 1.2f, 0.f},
				               CGameUtilMgr::GetRandomFloat(0.15f, 0.3f), RED, 20, 0.2f);

			if (m_vHurtSound.size() > 0)
				CSoundMgr::GetInstance()->PlaySoundRandom(m_vHurtSound, m_pOwnerTrans->m_vInfo[INFO_POS], 0.2f);
		}
	}

	if (m_iHP <= 0)
		m_bDead = true;

	if (m_iHP > (_int)m_iMaxHP)
		m_iHP = (_int)m_iMaxHP;

	m_DamageDelegater.broadcast(m_iHP, m_iMaxHP, iModifyingHP);
}

void CStatComponent::TakeDamage(_int iDamage, _vec3 vFromPos, CGameObject* pCauser, DamageType eType, _bool bCritical)
{
	if (m_bDead) return;

	if (g_bOnline)
	{
		CKouku* pKouku =  dynamic_cast<CKouku*>(m_pOwner);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pCauser);
		if (pKouku && pPlayer && pKouku->IsRemote() == false)
		{
			Protocol::C_KOUKU_DAMAGE damagePkt;
			damagePkt.mutable_player()->set_id(pPlayer->GetID());
			damagePkt.set_damage(iDamage);

			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(damagePkt));	
		}
	}


	switch (eType)
	{
	case DT_STUN:
		m_bStun = true;
		m_fCurStunTime = 0.f;
		m_pStun = CEffectFactory::Create<CStun>("Monster_Stun", L"Monster_Stun",
		                                        m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{0.f, 3.f, 0.f});
		break;
	case DT_KNOCK_BACK:
		m_bKnockback = true;
		m_fCurKnockbackTime = 0.f;
		m_fPreYPos = m_pOwnerTrans->m_vInfo[INFO_POS].y;

		m_vKnockBackVelocity = m_pOwnerTrans->m_vInfo[INFO_POS] - vFromPos;
		D3DXVec3Normalize(&m_vKnockBackVelocity, &m_vKnockBackVelocity);
		m_vKnockBackVelocity *= 10.f;
		m_vKnockBackVelocity.y = 10.f;
		break;
	case DT_HIGH_KNOCK_BACK:
		m_bKnockback = true;
		m_fCurHighKnockBackTime = 0.f;
		m_fCurKnockbackTime = 0.f;
		m_fPreYPos = m_pOwnerTrans->m_vInfo[INFO_POS].y;
		m_pOwnerTrans->m_vInfo[INFO_POS].y += 3.f;

		m_vKnockBackVelocity = m_pOwnerTrans->m_vInfo[INFO_POS] - vFromPos;
		D3DXVec3Normalize(&m_vKnockBackVelocity, &m_vKnockBackVelocity);
		m_vKnockBackVelocity *= 10.f;
		m_vKnockBackVelocity.y = 30.f;
		break;
	case DT_STIFFEN:
		m_bStiffen = true;
		m_fCurStiffeTime = 0.f;

		m_vKnockBackVelocity = m_pOwnerTrans->m_vInfo[INFO_POS] - vFromPos;
		D3DXVec3Normalize(&m_vKnockBackVelocity, &m_vKnockBackVelocity);
		m_vKnockBackVelocity *= 15.f;
		break;
	case DT_HUGE_KNOCK_BACK:
		m_bKnockback = true;
		m_fCurKnockbackTime = 0.f;
		m_fPreYPos = m_pOwnerTrans->m_vInfo[INFO_POS].y;

		m_vKnockBackVelocity = m_pOwnerTrans->m_vInfo[INFO_POS] - vFromPos;
		D3DXVec3Normalize(&m_vKnockBackVelocity, &m_vKnockBackVelocity);
		m_vKnockBackVelocity *= 45.f;
		m_vKnockBackVelocity.y = 18.f;
		break;
	case DT_KOUKU_SYMBOL_BLUE:
		m_bSatonSymbol_Blue = true;
		m_fCurSatonSymbolTime = 0.f;
		break;
	case DT_KOUKU_SYMBOL_RED:
		m_bSatonSymbol_Red = true;
		m_fCurSatonSymbolTime = 0.f;
		break;
	case DT_SATON_FASCINATED:
		if (m_bDead) break;
		m_bFascinated = true;
		m_fCurSatonFascinatedTime = 0.f;
		m_pFaci = CEffectFactory::Create<CFascinate>("Facinate", L"Facinate",
		                                             m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{0.f, 3.f, 0.f});
		break;
	case DT_SATON_GRAPED:
		m_bGraped = true;
		break;
	case DT_JUMP:
		break;
	case DT_END:
		break;
	default:
		IM_LOG("Error : Invalid DamageType!");
		break;
	}

	ModifyHP(-iDamage);
	if (iDamage != 0)
	{
		CDamageFontMgr::GetInstance()->Add_DamageFontFromWorld(
			iDamage,
			m_pOwnerTrans->m_vInfo[INFO_POS] + _vec3{0.f, 1.5f, 0.f},
			vFromPos,
			D3DCOLOR_ARGB(255, 255, 255, 255),
			bCritical);
	}
}

void CStatComponent::SetDead()
{
	ModifyHP(-(static_cast<_int>(m_iMaxHP)), false);
}

void CStatComponent::Revive()
{
	m_iHP = m_iMaxHP;
	_int tmp = 0;
	m_DamageDelegater.broadcast(m_iHP, m_iMaxHP, tmp);
	m_bDead = false;
	m_bStun = false;
	m_bDamaged = false;
	m_bKnockback = false;
	m_bSatonSymbol_Red = false;
	m_bSatonSymbol_Blue = false;
	m_bFascinated = false;
}
