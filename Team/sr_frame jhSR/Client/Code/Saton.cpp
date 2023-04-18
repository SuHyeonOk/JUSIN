#include "stdafx.h"
#include "Saton.h"
#include "AbstFactory.h"
#include "Cat_Attack.h"
#include "Kouku.h"
#include "Particle.h"
#include "SatonController.h"
#include "ServerPacketHandler.h"
#include "Skeleton.h"
#include "StatComponent.h"
#include "StaticCamera.h"

CSaton::CSaton(LPDIRECT3DDEVICE9 pGraphicDev) : CMonster(pGraphicDev)
{
}

CSaton::CSaton(const CMonster& rhs) : CMonster(rhs)
{
}

HRESULT CSaton::Ready_Object()
{
	CMonster::Ready_Object();
	m_pColl->SetCollType(COLL_WALL);

	m_arrAnim[INTRO] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_intro.anim");
	m_arrAnim[FIRSTATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_doubleattack.anim");
	// m_arrAnim[SECONDATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_attack2.anim");
	m_arrAnim[IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_idle.anim");
	m_arrAnim[SATON_BIRD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_bird.anim");
	m_arrAnim[SATON_GRAP] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_grap.anim");
	m_arrAnim[SATON_SYMBOL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_symbol.anim");
	m_arrAnim[SATON_FASCINATE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/saton_fascinate.anim");
	m_pIdleAnim = &m_arrAnim[IDLE];
	// m_pCurAnim = &m_arrAnim[INTRO];
	m_pCurAnim = m_pIdleAnim;

	m_eState = INTRO;
	m_fSpeed = 2.f;
	// m_pStat->IsSaton();
	m_pStat->SetMaxHP(100);
	// m_pRootPart->pTrans->Set_Pos(62.5f, 20.5f, 49.4f);
	m_pRootPart->pTrans->m_vInfo[INFO_POS].y = 20.5f;

	m_vTargetPos = _vec3(60.f,25.f,33.f);

	if (m_bRemote)
	{
		CController* pController = Add_Component<CSatonController>(L"Proto_SatonRemoteController", L"Proto_SatonRemoteController", ID_DYNAMIC);
		pController->SetOwner(this);
	}
	else
	{
		CController* pController = Add_Component<CSatonController>(L"Proto_SatonController", L"Proto_SatonController", ID_DYNAMIC);
		pController->SetOwner(this);
	}

	m_fCurTime = 0.f;
	m_fTime = 1.f;
	//cc면역
	m_bCantCC = true;
	m_bCanPlayAnim = false;
	PlayAnimationOnce(&m_arrAnim[IDLE]);

	return S_OK;
}

void CSaton::AnimationEvent(const string& strEvent)
{
	if (strEvent == "FirstAttack")
	{
		m_bFirstHammerAttack = true;
	}
	else if (strEvent == "SatonGrap")
	{
		m_bSatonGrap = true;
	}
	else if (strEvent == "SatonBird")
	{
		m_bSatonBird = true;
	}
	else if (strEvent == "SymbolAttack")
	{
		// m_bSatonSymbol_Blue = true;
	}
	else if (strEvent == "AnimStopped")
	{
		m_bDelete = true;
	}
	else if (strEvent == "Ready_Attack_1")
	{
			
		m_vATKRNGCirclePos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.8f;
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vATKRNGCirclePos
			, READY_CIRCLE, CGameUtilMgr::s_vZero, _vec3(5.f, 5.f, 5.f), 37, 37);

		
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vATKRNGCirclePos
			, ATTACK_CIRCLE, CGameUtilMgr::s_vZero, _vec3(5.f, 5.f, 5.f), 130, 37);
	}
	else if (strEvent == "Ready_Attack_2")
	{
		 
		m_vATKRNGCirclePos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.8f;
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vATKRNGCirclePos
			, READY_CIRCLE, CGameUtilMgr::s_vZero, _vec3(6.f, 6.f, 6.f), 63, 63);

		
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vATKRNGCirclePos
			, ATTACK_CIRCLE, CGameUtilMgr::s_vZero, _vec3(6.f, 6.f, 6.f), 130, 63);
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayShake(0.15f, 0.4f);

		CEffectFactory::Create<CUVCircle>("Hammer1_Explosion", L"Hammer1_Explosion",
			_vec3(m_vATKRNGCirclePos.x, m_vATKRNGCirclePos.y + 4.2f, m_vATKRNGCirclePos.z));
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"Saton_Particle")->Add_Particle(_vec3(m_vATKRNGCirclePos.x, m_vATKRNGCirclePos.y + 4.2f, m_vATKRNGCirclePos.z), 0.5f, D3DXCOLOR(1.0f,0.2f,0.f,0.f), 15, 0.5f);
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"Saton_Particle")->Add_Particle(_vec3(m_vATKRNGCirclePos.x, m_vATKRNGCirclePos.y + 4.2f, m_vATKRNGCirclePos.z), 0.7f, D3DXCOLOR(0.7f, 0.1f, 0.2f, 0.f), 15, 0.5f);

		m_bIsAttack_1_Coll = true;
	}
	else if (strEvent == "Attack_2_End")
	{
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayShake(0.2f, 0.4f);
		CEffectFactory::Create<CUVCircle>("Hammer2_Explosion", L"Hammer2_Explosion",
			_vec3(m_vATKRNGCirclePos.x, m_vATKRNGCirclePos.y + 4.2f, m_vATKRNGCirclePos.z));
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"Saton_Particle")->Add_Particle(_vec3(m_vATKRNGCirclePos.x, m_vATKRNGCirclePos.y + 4.2f, m_vATKRNGCirclePos.z), 0.5f, D3DXCOLOR(1.0f, 0.2f, 0.f, 0.f), 15, 0.5f);
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"Saton_Particle")->Add_Particle(_vec3(m_vATKRNGCirclePos.x, m_vATKRNGCirclePos.y + 4.2f, m_vATKRNGCirclePos.z), 0.7f, D3DXCOLOR(0.7f, 0.1f, 0.2f, 0.f), 15, 0.5f);

		m_bIsAttack_2_Coll = true;

	}
	else if (strEvent == "Second_Attack")
	{
		// 어택 써클만 생성
	}
	else if(strEvent == "Grap_Start")
	{
		IM_LOG("Grap_Start");
		m_bIsGrap = true;
		CSoundMgr::GetInstance()->PlaySound(L"grab_attack.wav", _vec3{ 62.5f,25.f,40.5f });
	}
	else if (strEvent == "Grap_End")
	{
		IM_LOG("Grap End");
		m_bIsGrap = false;
		m_bIsGrapEndAttack = true;
	}
	else if (strEvent == "Cat_Spawn")
	{
		_matrix mat = Get_SkeletalPart("hat_bot")->GetWorldMat();
		_vec3 v_scale = CGameUtilMgr::s_vOne;
		_vec3 vBoriPos = _vec3(mat._41 + CGameUtilMgr::GetRandomFloat(-2.5f,2.5f), mat._42 - 0.5f, mat._43 + CGameUtilMgr::GetRandomFloat(-0.5f, 0.5f));
		_vec3 vRuiPos = _vec3(mat._41 + CGameUtilMgr::GetRandomFloat(-2.5f, 2.5f), mat._42 - 0.5f, mat._43 + CGameUtilMgr::GetRandomFloat(-0.5f, 0.5f));
		_vec3 vHoddeukPos = _vec3(mat._41 + CGameUtilMgr::GetRandomFloat(-2.5f, 2.5f), mat._42 - 0.5f, mat._43 + CGameUtilMgr::GetRandomFloat(-0.5f, 0.5f));
		
		_vec3 vAngle = m_pRootPart->pTrans->m_vAngle;
		_matrix matBoriWorld, matRuiWorld, matHoddeukWorld;
		CGameUtilMgr::MatWorldComposeEuler(matBoriWorld,  v_scale ,  vAngle , vBoriPos);
		CGameUtilMgr::MatWorldComposeEuler(matRuiWorld, v_scale, vAngle, vRuiPos);
		CGameUtilMgr::MatWorldComposeEuler(matHoddeukWorld, v_scale, vAngle, vHoddeukPos);
		
		CObjectFactory::Create<CCat_Attack>("Bori", L"Bori", matBoriWorld);
		CObjectFactory::Create<CCat_Attack>("Rui", L"Rui", matRuiWorld);
		CObjectFactory::Create<CCat_Attack>("Hoddeuk", L"Hoddeuk", matHoddeukWorld);
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"catspawn1_1.wav", L"catspawn1_2.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);

		m_bIsCatColl = true; 
	}
	else if(strEvent == "Cat_Dead")
	{
		m_bIsCatColl = false;
	}
	else if (strEvent == "ExplodeMoon") // 바닥에 있던 달 위치에 쇼크파우더 뿌리기 
	{

		m_bStatonExplodeMoon = true;
	}
	else if (strEvent == "FascinateEnd")
	{
		PlayAnimationOnce(&m_arrAnim[FIRSTATTACK]);
	}
}

_int CSaton::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;


	CMonster::Update_Object(fTimeDelta);

	if (m_pCurAnim == m_pIdleAnim) // 이전 애니메이션 종료
		m_bCanPlayAnim = true;	

	if(m_bSatonDrawMoon)
	{
		m_fCurTime += fTimeDelta;

		if (m_fCurTime >= m_fTime)
		{
			m_bSatonDrawMoonPair = true;
			m_fCurTime = 0.f;
		}
	}

	_float tmp = D3DXToDegree(m_pRootPart->pTrans->m_vAngle.y);



	// 상태 변경 조건 설정
	StateChange();

	// 각 상태에 따른 프레임 마다 실행할 함수 지정
	switch (m_eState)
	{
	case INTRO:
		m_strState = "INTRO";
		break;
	case IDLE:
		// m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		m_strState = "IDLE";
		break;
	case FIRSTATTACK:
		m_strState = "FIRSTATTACK";
		break;
	case SATON_BIRD:
		m_strState = "SATON_BIRD";
		break;
	case SATON_GRAP:
		m_strState = "SATON_GRAP";
		break;
	case SATON_SYMBOL:
		m_strState = "SATON_SYMBOL";
		break;
	case SATON_FASCINATE:
		m_strState = "SATON_FASCINATE";
		break;
	case DEAD:
		m_strState = "DEAD";
	default:
		break;
	}
	if (m_bSatonIntro) return OBJ_NOEVENT;

// #ifdef _DEBUG
// 	IM_BEGIN("StatePos");
//
// 	ImGui::Text("%f,%f,%f",m_pRootPart->pTrans->m_vInfo[INFO_POS].x, m_pRootPart->pTrans->m_vInfo[INFO_POS].y, m_pRootPart->pTrans->m_vInfo[INFO_POS].z);
//
// 	IM_END;
// #endif


	return OBJ_NOEVENT;
}

void CSaton::LateUpdate_Object()
{
	CMonster::LateUpdate_Object();

	if (m_bStatonExplodeMoon)
	{
		set<CGameObject*> setPlayer;
		Engine::GetOverlappedObject(setPlayer, m_vExplodMoonPos, 4.f);
		DEBUG_SPHERE(m_vExplodMoonPos, 4.f, 1.f);
		for (auto& obj : setPlayer)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				CStatComponent* PlayerStatCom = pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC);
				PlayerStatCom->TakeDamage(0, pPlayer->GetInfo(INFO_POS), this, DT_STUN);
			}
		}
		for (int j = 0; j < 10; j++)
		{
			CEffectFactory::Create<CShock_Powder>("Shock_Powder", L"UV_Shock_Powder",
				_vec3(m_vExplodMoonPos.x + CGameUtilMgr::GetRandomFloat(-3.f,3.f),
					m_vExplodMoonPos.y, m_vExplodMoonPos.z + CGameUtilMgr::GetRandomFloat(-3.f, 3.f)));
			CEffectFactory::Create<CCloud>("ShockPowder_Cloud", L"ShockPowder_Cloud", m_vExplodMoonPos);
		}
		CEffectFactory::Create<CUVCircle>("Shock_Circle", L"Shock_Circle", m_vExplodMoonPos);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_shockpowder-001.ogg", m_vExplodMoonPos);
		m_bStatonExplodeMoon = false;
	}


	if(m_bSatonDrawMoon && m_bSatonDrawMoonPair)
	{
		set<CGameObject*> setPlayer;
		_vec3 SatonPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];

		Engine::GetOverlappedObject(setPlayer, SatonPos, 100.f);

		for (auto& obj : setPlayer)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				CStatComponent* PlayerStatCom = pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC);
				PlayerStatCom->TakeDamage(0, pPlayer->GetInfo(INFO_POS), this, DT_SATON_FASCINATED);
			}
		}
		m_bSatonDrawMoon = false;
		m_bSatonDrawMoonPair = false;
	}

	if(m_bIsAttack_1_Coll)
	{
		_vec3& vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.8f;
		
		set<CGameObject*> setObj;
		Engine::GetOverlappedObject(setObj, m_vATKRNGCirclePos, 5.f);
		
		for (auto& obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(15, vPos, this, DT_HUGE_KNOCK_BACK);
				IM_LOG("damage");
				break;
			}
		}
		m_bIsAttack_1_Coll = false;
		CSoundMgr::GetInstance()->PlaySound(L"attack2_hit_1.wav", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
	}

	if(m_bIsAttack_2_Coll)
	{
			_vec3& vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.8f;

			set<CGameObject*> setObj;
			Engine::GetOverlappedObject(setObj, m_vATKRNGCirclePos, 6.f);

			for (auto& obj : setObj)
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				{
					pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
						->TakeDamage(25, vPos, this, DT_HUGE_KNOCK_BACK);
					IM_LOG("damage");
					break;
				}
			}
			m_bIsAttack_2_Coll = false;
	}

	if (m_bIsGrap)
	{
		_matrix mat = Get_SkeletalPart("hand")->GetWorldMat();

		_vec3 vPos = _vec3(mat._41, mat._42, mat._43);
		set<CGameObject*> setObj;
		Engine::GetOverlappedObject(setObj, vPos, 3.f);

		for (auto& obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(0, vPos, this, DT_SATON_GRAPED);
				pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
					->Set_Pos(vPos.x,vPos.y-1.5f,vPos.z);


				m_pGrabbedList.insert(pPlayer);
			}
		}
		DEBUG_SPHERE(vPos, 3.f, 0.1f);
	}

	if (m_bIsGrapEndAttack)
	{
		_matrix mat = Get_SkeletalPart("hand")->GetWorldMat();

		_vec3 vPos = _vec3(mat._41, mat._42, mat._43);

		for (auto& pPlayer : m_pGrabbedList)
		{
	
			pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(50, m_pRootPart->pTrans->m_vInfo[INFO_POS], this, DT_HIGH_KNOCK_BACK);
			// pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
			// 	->Set_Pos(vPos.x, vPos.y - 1.5f, vPos.z);
			pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->Graped_Off();
		}
		m_pGrabbedList.clear();

		m_bIsGrapEndAttack = false;
	}

	if(m_bIsCatColl)
	{
		_vec3& vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.f;
		_vec3& vPos_2 = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 4.f;
		vPos.y = 25.f;
		vPos_2.y = 25.f;

		set<CGameObject*> setObj1,setObj2;
		Engine::GetOverlappedObject(setObj1, vPos, 3.f);
		Engine::GetOverlappedObject(setObj2, vPos_2, 3.f);

		for (auto& obj : setObj1)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(20, vPos, this, DT_KNOCK_BACK);

				IM_LOG("damage");
				break;
			}
		}
		DEBUG_SPHERE(vPos, 3.f, 0.1f);
	

	for (auto& obj : setObj2)
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
		{
			pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(30, vPos_2, this, DT_KNOCK_BACK);

			IM_LOG("damage");
			break;
		}
	}
		DEBUG_SPHERE(vPos_2, 3.f, 0.1f);
	}
}



	








void CSaton::Free()
{
	CMonster::Free();
}

CSaton* CSaton::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote)
{
	CSaton* pInstance = new CSaton(pGraphicDev);
	pInstance->m_bRemote = bRemote;
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CSaton::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			m_eState = DEAD;
			PlayAnimationOnce(&m_arrAnim[DEAD], true);
			m_bCanPlayAnim = false;
			return;
		}
	}

	if (m_bSatonIntro && m_bCanPlayAnim)
	{
		CSoundMgr::GetInstance()->PlaySound(L"Saton_Intro_0_2_1_1.wav", { 59.5f, 25.f ,35.5f });
		m_eState = INTRO;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[INTRO]);
		m_bCanPlayAnim = false;
		// SetOff();
		m_bSatonIntro = false;
		return;
	}

	if (m_bFirstHammerAttack && m_bCanPlayAnim)
	{
		m_eState = FIRSTATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[FIRSTATTACK]);
		m_bCanPlayAnim = false;
		Protocol::C_DEBUG_PKT pkt;
		string debug = "Player : " +  to_string(CClientServiceMgr::GetInstance()->m_iPlayerID) + " Hit x :"
		+ to_string(m_vTargetPos.x) +  ", z :" + to_string(m_vTargetPos.z);
		pkt.set_debuglog(debug);
		CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(pkt));	
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"attack2_big.wav", L"attack2_big_1.wav" }, { 59.5f, 25.f ,40.5f });
		SetOff();
		return;
	}

	if (m_bSatonBird && m_bCanPlayAnim)
	{
		m_eState = SATON_BIRD;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SATON_BIRD]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}

	if (m_bSatonGrap && m_bCanPlayAnim)
	{
		m_eState = SATON_GRAP;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SATON_GRAP]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}
	if (m_bSatonSymbolAnim && m_bCanPlayAnim && m_eState == IDLE)
	{
		m_eState = SATON_SYMBOL;
		CSoundMgr::GetInstance()->PlaySound(L"fear1.wav", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SATON_SYMBOL]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}
	if (m_bSatonFascinate && m_bCanPlayAnim)
	{
		m_eState = SATON_FASCINATE;
		CSoundMgr::GetInstance()->PlaySound(L"hook.wav", m_pRootPart->pTrans->m_vInfo[INFO_POS]);

		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SATON_FASCINATE]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}
	if (m_bCanPlayAnim)
	{
		m_eState = IDLE;
		RotateToTargetPos(m_vTargetPos);
		m_pIdleAnim = &m_arrAnim[IDLE];
		m_pCurAnim = &m_arrAnim[IDLE];
		return;
	}
}

void CSaton::SatonShockPowder()
{

}

CSaton::~CSaton()
{
}
