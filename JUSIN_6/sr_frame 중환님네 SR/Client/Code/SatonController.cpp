#include "stdafx.h"
#include "SatonController.h"

#include "Kouku.h"
#include "Saton.h"
#include "ServerPacketHandler.h"
#include "StatComponent.h"

CSatonController::CSatonController()
{
	//67
	m_fSatonFascinateCoolTime = 35.f;
	m_fSatonGrapCoolTime = 19.f;
	m_fSatonBirdCoolTime = 27.f;
	m_fFirstHammerCoolTime = 11.f;
}

CSatonController::CSatonController(const CSatonController& rhs)
{
	m_fSatonFascinateCoolTime = 35.f;
	m_fSatonGrapCoolTime = 19.f;
	m_fSatonBirdCoolTime = 27.f;
	m_fFirstHammerCoolTime = 11.f;
}

CSatonController::~CSatonController()
{
}

_int CSatonController::Update_Component(const _float& fTimeDelta)
{
	{
		// m_fCurLookAtTime += fTimeDelta;
		m_fCurFirstHammerCoolTime += fTimeDelta;
		m_fCurSatonBirdCoolTime += fTimeDelta;
		m_fCurSatonGrapCoolTime += fTimeDelta;
		m_fCurSatonFascinateCoolTime += fTimeDelta;

		if(m_bIsDrawMoon)
		{
			m_fCurSatonDrawMoonCoolTime += fTimeDelta;
		}
	}

// #ifdef _DEBUG
// 	IM_BEGIN("Saton_CoolTime");
// 	ImGui::Text("Fascinate Cur CoolTime : %f", m_fCurSatonFascinateCoolTime);
// 	IM_END;
// #endif


	CSaton* saton = dynamic_cast<CSaton*>(m_pOwner);
	NULL_CHECK_RETURN(saton, 0);

	_vec3 vPos = saton->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	_vec3 vTargetPos;
	// _float fTargetDist = 9999.f;
	if (!m_bIsGimmick_On)
	{
		if (m_fCurTargetingCoolTime > m_fTargetingCoolTime)
		{
			for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
				{
					if (pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)->IsDead())
						continue;

					vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
					_vec3 vDiff = vTargetPos - vPos;
					_float fDist = D3DXVec3Length(&vDiff);

					if (m_fTargetDist > fDist) // 플레이어 감지
					{
						m_pTargetPlayer = pPlayer;
						m_fTargetDist = fDist;
					}
				}
			}
			m_fCurTargetingCoolTime = 0.f;

			if (m_fTargetDist > 20.f)
				m_pTargetPlayer = nullptr;
		}
		else
		{
			m_fCurTargetingCoolTime += fTimeDelta;
			if (m_pTargetPlayer == nullptr) return 0;

			vTargetPos = m_pTargetPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDiff = vTargetPos - vPos;
			_float fDist = D3DXVec3Length(&vDiff);
		}
	}

	CKouku* pKouku = Get_GameObjectUnCheck<CKouku>(LAYER_ENEMY, L"Kouku");
	if (pKouku == nullptr) 
	{
		saton->IsDeadTrue();
		return 0;
	}
	m_vLookFront = saton->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_LOOK] - _vec3(62.5f, 0, 45.f);
	_vec3 vLookFront = pKouku->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	if(pKouku->Check_SymbolGimmick() && !m_bIsKoukuSymbol)
	{
		saton->SatonSymbolAnim(_vec3(m_vLookFront));
		m_bIsKoukuSymbol = !m_bIsKoukuSymbol;

		if (g_bOnline)
		{
			Protocol::C_SATON_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::SYMBOL);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
	}
	
	if (m_fCurFirstHammerCoolTime >= m_fFirstHammerCoolTime && m_fTargetDist <= m_fFirstHammerDist)
	{
		m_fCurFirstHammerCoolTime = 0.f;
		saton->FirstAttack(vTargetPos);
		if (g_bOnline)
		{
			Protocol::C_SATON_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::HAMMER);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}

		return 0;
	}
	if (m_fCurSatonBirdCoolTime >= m_fSatonBirdCoolTime && m_fTargetDist <= m_fSatonBirdkDist)
	{
		m_fCurSatonBirdCoolTime = 0.f;
		saton->SatonBird(vTargetPos);
		if (g_bOnline)
		{
			Protocol::C_SATON_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::BIRD);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}

	if (m_fCurSatonFascinateCoolTime >= m_fSatonFascinateCoolTime && m_fTargetDist <= m_fSatonFascinateDist)
	{
		m_fCurFirstHammerCoolTime -= 15.f;
		m_fCurSatonBirdCoolTime -= 15.f;
		m_fCurSatonGrapCoolTime -= 15.f;
		m_fSatonFascinateCoolTime = 99999.f;
		for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second);
			if (pPlayer && pPlayer->GetID() == 0 && pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)->IsDead())
			{
				m_fCurSatonFascinateCoolTime -= 10.f;
				return 0;
			}
		}

		m_fCurSatonFascinateCoolTime = 0.f;
		saton->SatonFascinate(m_vLookFront, vTargetPos);
		m_bIsDrawMoon = true;

		if (g_bOnline)
		{
			Protocol::C_SATON_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::FASCINATE);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}
	
	if(m_fCurSatonDrawMoonCoolTime >= m_fSatonDrawMoonCoolTime && m_fTargetDist <= m_fSatonFascinateDist)
	{
		m_fCurSatonDrawMoonCoolTime = 0.f;

		for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second))
			{
				if (pPlayer->GetID() == 0)
				{
					vTargetPos = pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
					break;
				}
			}
		}

		saton->SatonDrawMoon(vTargetPos);
		if (g_bOnline)
		{
			Protocol::C_SATON_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::DRAWMOON);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		m_bIsDrawMoon = false;
	}

	if (m_fCurSatonGrapCoolTime >= m_fSatonGrapCoolTime && m_fTargetDist <= m_fSatonGrapkDist)
	{
		m_fCurSatonGrapCoolTime = 0.f;
		saton->SatonGrap(vLookFront);
		if (g_bOnline)
		{
			Protocol::C_SATON_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::GRAB);
			patternPkt.mutable_targetpos()->set_x(vLookFront.x);
			patternPkt.mutable_targetpos()->set_y(vLookFront.y);
			patternPkt.mutable_targetpos()->set_z(vLookFront.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}

	if (m_fCurLookAtTime >= m_fLookAtTime)
	{
		m_fCurLookAtTime = 0.f;
		// _vec3  Pos = saton->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS] - _vec3(62.5f, 21.5f, 47.f);
		// saton->WalkToTarget(vTargetPos);
		//
		// if (g_bOnline)
		// {
		// 	Protocol::C_SATON_ATTACK patternPkt;
		// 	patternPkt.set_pattern(Protocol::MoveTo);
		// 	patternPkt.mutable_targetpos()->set_x(vLookFront.x);
		// 	patternPkt.mutable_targetpos()->set_y(vLookFront.y);
		// 	patternPkt.mutable_targetpos()->set_z(vLookFront.z);
		// 	CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		// }
	}


	return 0;
}

CComponent* CSatonController::Clone()
{
	return new CSatonController(*this);
}

void CSatonController::Free()
{
	CController::Free();
}

CSatonController* CSatonController::Create()
{
	return new CSatonController;
}

// remote
CSatonRemoteController::CSatonRemoteController() : CSatonController()
{
}

CSatonRemoteController::CSatonRemoteController(const CSatonRemoteController& rhs) : CSatonController(rhs)
{
}

CSatonRemoteController::~CSatonRemoteController()
{
}

_int CSatonRemoteController::Update_Component(const _float& fTimeDelta)
{
	CSaton* saton = dynamic_cast<CSaton*>(m_pOwner);
	NULL_CHECK_RETURN(saton, 0);


	if (m_patternList.empty() == false)
	{
		pair<_vec3, Protocol::SatonPattern> pattern;
		{
			WRITE_LOCK;

			pattern = m_patternList.front();
			m_patternList.pop_front();
			m_patternList.clear();
		}

		switch (pattern.second)
		{
			case Protocol::MoveTo:
				saton->WalkToTarget(pattern.first);
				break;
			case Protocol::HAMMER:
				saton->FirstAttack(pattern.first);
				break;
			case Protocol::GRAB:
				saton->SatonGrap(pattern.first);
				break;
			case Protocol::BIRD:
				saton->SatonBird(pattern.first);
				break;
			case Protocol::SYMBOL:
				m_vLookFront = saton->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_LOOK] - _vec3(62.5f, 0, 45.f);
				saton->SatonSymbolAnim(m_vLookFront);
				break;
			case Protocol::FASCINATE:
				m_vLookFront = saton->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_LOOK] - _vec3(62.5f, 0, 45.f);
				saton->SatonFascinate(m_vLookFront, pattern.first);
				break;
			case Protocol::DRAWMOON:
				saton->SatonDrawMoon(pattern.first);
				break;
			case Protocol::SatonPattern_INT_MIN_SENTINEL_DO_NOT_USE_: break;
			case Protocol::SatonPattern_INT_MAX_SENTINEL_DO_NOT_USE_: break;
			default: ;
		}



	}


	return OBJ_NOEVENT;
}

CComponent* CSatonRemoteController::Clone()
{
	return new CSatonRemoteController(*this);
}

void CSatonRemoteController::Free()
{
	CSatonController::Free();
}

CSatonRemoteController* CSatonRemoteController::Create()
{
	return new CSatonRemoteController;
}

