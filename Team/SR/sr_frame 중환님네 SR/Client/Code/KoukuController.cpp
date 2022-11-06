#include "stdafx.h"
#include "KoukuController.h"

#include "Kouku.h"
#include "ServerPacketHandler.h"
#include "StatComponent.h"


CKoukuController::CKoukuController()
{
	m_fBasicAttackCoolTime = 5.f;
	m_fDoubleHammerCoolTime = 21.f;
	m_fHorrorAttackCoolTime = 29.f;
}

CKoukuController::CKoukuController(const CKoukuController& rhs)
{
	m_fBasicAttackCoolTime = 5.f;
	m_fDoubleHammerCoolTime = 21.f;
	m_fHorrorAttackCoolTime = 29.f;
}

CKoukuController::~CKoukuController()
{
}

_int CKoukuController::Update_Component(const _float& fTimeDelta)
{
	{
		m_fCurMoveToTime += fTimeDelta;

		m_fCurDoubleHammerCoolTime += fTimeDelta;
		m_fCurHorrorAttackCoolTime += fTimeDelta;
		m_fCurBasicAttackCoolTime += fTimeDelta;

		if (m_bIsSymbolGimmick)
		{
			m_fCurSymbolGimmickCoolTime += fTimeDelta;
		}
	}
	CKouku* pKouku = dynamic_cast<CKouku*>(m_pOwner);
	NULL_CHECK_RETURN(pKouku, 0);

	_vec3 vPos = pKouku->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	_vec3 vTargetPos;
	// _float fTargetDist = 9999.f;

	if (m_fCurTargetingCoolTime > m_fTargetingCoolTime)
	{
		m_pTargetPlayer = nullptr;
		m_fTargetDist = 9999.f;
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
		m_fTargetDist = D3DXVec3Length(&vDiff);
	}

	CStatComponent* pkouku = Engine::Get_Component<CStatComponent>(LAYER_ENEMY, L"Kouku", L"Proto_StatCom", ID_DYNAMIC);
	
	const _int koukuMaxHP = pkouku->GetMaxHP();
	const _int koukuHP = pkouku->GetHP();

	if(koukuHP <= koukuMaxHP / 2 && !m_bIsFirstSymbolGimmick)
	{
		pKouku->KoukuSymbol(vTargetPos);
		m_strState = "KoukuSymbol_On";
		pKouku->KoukuSymbol_OnOff(true);
		m_bIsFirstSymbolGimmick = true;
		m_bIsSymbolGimmick = true;

		if (g_bOnline)
		{
			Protocol::C_KOUKU_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::KOUKU_SYMBOL_ON);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
	}
	// 심볼 기믹false로 돌려줘야함

	if (m_fCurSymbolGimmickCoolTime >= m_fSymbolGimmickCoolTime && m_bIsSymbolGimmick)
	{
		m_fCurSymbolGimmickCoolTime = 0.f;
		pKouku->KoukuSymbol_OnOff(false);
		m_bIsSymbolGimmick = false;

		if (g_bOnline)
		{
			Protocol::C_KOUKU_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::KOUKU_SYMBOL_OFF);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}

	if (m_fCurDoubleHammerCoolTime >= m_fDoubleHammerCoolTime && m_fTargetDist <= m_fDoubleHammerDist)
	{
		m_fCurDoubleHammerCoolTime = 0.f;
		pKouku->DoubleHammer(vTargetPos);
		m_strState = "DoubleHammer_On";

		if (g_bOnline)
		{
			Protocol::C_KOUKU_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::KOUKU_HAMMER);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}

	if (m_fCurBasicAttackCoolTime >= m_fBasicAttackCoolTime && m_fTargetDist <= m_fBasicAttackDist)
	{
		m_fCurBasicAttackCoolTime = 0.f;
		pKouku->BasicAttack(vTargetPos);
		m_strState = "BasicAttack_On";
		if (g_bOnline)
		{
			Protocol::C_KOUKU_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::KOUKU_BASIC);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}

	if (m_fCurHorrorAttackCoolTime >= m_fHorrorAttackCoolTime && m_fTargetDist <= m_fHorrorAttackDist)
	{
		m_fCurHorrorAttackCoolTime = 0.f;
		pKouku->HorrorAttack(vTargetPos);
		m_strState = "HorrorAttack_On";
		if (g_bOnline)
		{
			Protocol::C_KOUKU_ATTACK patternPkt;
			patternPkt.set_pattern(Protocol::KOUKU_HORROR);
			patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
			patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
			patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
			CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));	
		}
		return 0;
	}

// #ifdef _DEBUG
// 	IM_BEGIN("test");
// 	ImGui::Text("%f", m_fTargetDist);
// 	IM_END;
// #endif


	if (m_fCurMoveToTime >= m_fMoveToTime)
	{
		m_fCurMoveToTime = 0.f;

		if (m_fTargetDist > 3.f)
		{
			pKouku->WalkToTarget(vTargetPos);

			if (g_bOnline)
			{
				Protocol::C_KOUKU_ATTACK patternPkt;
				patternPkt.set_pattern(Protocol::KOUKU_MoveTo);
				patternPkt.mutable_targetpos()->set_x(vTargetPos.x);
				patternPkt.mutable_targetpos()->set_y(vTargetPos.y);
				patternPkt.mutable_targetpos()->set_z(vTargetPos.z);
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(patternPkt));


				Protocol::C_BOSS_WORLD bossWorldPkt;
				bossWorldPkt.set_objkey("Kouku");
				const _matrix& matWorld = pKouku->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
				CClientServiceMgr::Mat2Pkt(matWorld, *bossWorldPkt.mutable_matworld());
				bossWorldPkt.set_ihp((_uint)koukuHP);
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(bossWorldPkt));
			}
		}
	}

	
		
	// IM_BEGIN("Kouku_ControlloerData");
	//
	// ImGui::Text("CurDoubleHammerCoolTime : %f,DoubleHammerCoolTime : %f", m_fCurDoubleHammerCoolTime, m_fDoubleHammerCoolTime);
	// // ImGui::Text("m_bIsSymbolGimmick", m_bIsSymbolGimmick);
	// // ImGui::Text("%s, ", m_strState.c_str());
	// // ImGui::Text("%d", m_pStat->GetHP());
	// // ImGui::Text("%d", m_pStat->GetHP());
	// // ImGui::Text("%d", m_pStat->GetHP());
	// // ImGui::Text("%d", m_pStat->GetHP());
	// // ImGui::Text("%d", m_pStat->GetHP());
	//
	// ImGui::Separator();
	//
	// IM_END;


	return 0;
}

CComponent* CKoukuController::Clone()
{
	return new CKoukuController(*this);
}

void CKoukuController::Free()
{
	CController::Free();
}

CKoukuController* CKoukuController::Create()
{
	return new CKoukuController;
}

// remote

CKoukuRemoteController::CKoukuRemoteController() : CKoukuController()
{
}

CKoukuRemoteController::CKoukuRemoteController(const CKoukuRemoteController& rhs) : CKoukuController(rhs)
{
}

CKoukuRemoteController::~CKoukuRemoteController()
{
}

_int CKoukuRemoteController::Update_Component(const _float& fTimeDelta)
{
	CKouku* pKouku = dynamic_cast<CKouku*>(m_pOwner);
	NULL_CHECK_RETURN(pKouku, 0);

	if (m_bWorldSet)
	{
		pKouku->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
			->Set_WorldDecompose(m_matWorld);
		pKouku->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
			->SetHP(m_iHP);

		m_bWorldSet.store(false);
	}

	if (m_patternList.empty() == false)
	{
		pair<_vec3, Protocol::KoukuPattern> pattern;
		{
			WRITE_LOCK;
			pattern = m_patternList.front();
			m_patternList.pop_front();
		}

		switch (pattern.second)
		{
			case Protocol::KOUKU_MoveTo:
				pKouku->WalkToTarget(pattern.first);
				break;
			case Protocol::KOUKU_SYMBOL_ON:
				pKouku->KoukuSymbol(pattern.first);
				pKouku->KoukuSymbol_OnOff(true);
				break;
			case Protocol::KOUKU_SYMBOL_OFF:
				pKouku->KoukuSymbol_OnOff(false);
				break;
			case Protocol::KOUKU_HAMMER:
				pKouku->DoubleHammer(pattern.first);
				break;
			case Protocol::KOUKU_HORROR:
				pKouku->HorrorAttack(pattern.first);
				break;
			case Protocol::KOUKU_BASIC:
				pKouku->BasicAttack(pattern.first);
				break;
			case Protocol::KoukuPattern_INT_MIN_SENTINEL_DO_NOT_USE_: break;
			case Protocol::KoukuPattern_INT_MAX_SENTINEL_DO_NOT_USE_: break;
			default: ;
		}
	}

	return 0;
}

CComponent* CKoukuRemoteController::Clone()
{
	return new CKoukuRemoteController(*this);
}

void CKoukuRemoteController::Free()
{
	CKoukuController::Free();
}

CKoukuRemoteController* CKoukuRemoteController::Create()
{
	return new CKoukuRemoteController;
}
