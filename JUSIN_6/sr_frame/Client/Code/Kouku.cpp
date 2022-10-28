#include "stdafx.h"
#include "..\Header\Kouku.h"
#include "AbstFactory.h"
#include "ClearUI.h"
#include "Skeleton.h"
#include "StatComponent.h"
#include "KoukuController.h"
#include "Particle.h"
#include "StaticCamera.h"
#include "Weapon.h"
#include "KoukuHpUI.h"
#include "ServerPacketHandler.h"
#include "Stage_Kouku.h"
#include "ObjectStoreMgr.h"

CKouku::CKouku(LPDIRECT3DDEVICE9 pGraphicDev) : CMonster(pGraphicDev)
{
}

CKouku::CKouku(const CMonster& rhs) : CMonster(rhs)
{
}

CKouku::~CKouku()
{
	CSoundMgr::GetInstance()->StopAll();
}

HRESULT CKouku::Ready_Object()
{
	CMonster::Ready_Object();
	m_fCurTrailTime = 0.005f;
	m_fTrailTime = 0.005f;
	m_arrAnim[INTRO] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_intro.anim");
	m_arrAnim[WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_walk.anim");
	m_arrAnim[BASIC_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_basicattack.anim");
	m_arrAnim[IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_idle.anim");
	m_arrAnim[HAMMER_IN] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_hammer_x.anim");
	m_arrAnim[HAMMER_OUT] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_hammer_o.anim");
	m_arrAnim[HAMMER_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_hamer.anim");
	m_arrAnim[HORROR_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_runattack.anim");
	m_arrAnim[SYMBOL_HIDE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_hide.anim");
	m_arrAnim[REST] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_rest.anim");
	m_arrAnim[STUN] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_stun.anim");
	m_arrAnim[DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/KoukuSaton/kouku_dead.anim");

	m_pIdleAnim = &m_arrAnim[IDLE];
	m_pCurAnim = &m_arrAnim[INTRO];
	// m_pCurAnim = m_pIdleAnim;
	m_vTargetPos = CGameUtilMgr::s_vZero + _vec3{ 67.5f,25.f,49.5f };
	m_eState = IDLE;
	m_fSpeed = 2.f;

	m_iRedSymbolCnt = 0;

	m_bIntroPlay = true;

	m_pStat->SetMaxHP(12000);


	if (m_bRemote)
	{
		CController* pController = Add_Component<CKoukuRemoteController>(L"Proto_KoukuRemoteController", L"Proto_KoukuRemoteController", ID_DYNAMIC);
		pController->SetOwner(this);
	}
	else
	{
		CController* pController = Add_Component<CKoukuController>(L"Proto_KoukuController", L"Proto_KoukuController", ID_DYNAMIC);
		pController->SetOwner(this);
	}

	m_fCurTime = 2.f;
	m_fTime = 1.1f;
	//cc면역
	m_bCantCC = true;

	m_fCurLightTime = 1.f;
	m_fLightTime = 0.08f;
	m_pBossHPUI = CUIFactory::Create<CKoukuHpUI>("KoukuHPUI", L"KoukuHPUI", -1, WINCX * 0.5f, WINCY * 0.15f, 500, 25);
	m_pBossHPUI->SetOwner(L"쿠 크", this, m_pStat->GetMaxHP());
	m_pBossHPUI->SetNamePos(0.47f);

	// m_bCanPlayAnim = false;
	// PlayAnimationOnce(&m_arrAnim[IDLE]);

	return S_OK;
}

void CKouku::AnimationEvent(const string& strEvent)
{
	if (strEvent == "SymbolCloud")
	{
		for (int i = 0; i < 5; i++)
		{
			CEffectFactory::Create<CCloud>("Creeper_Cloud", L"Creeper_Cloud", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		}
	}
	// else if (strEvent == "HorrorAttack")
	// {
	// 	m_bHorrorAttack = true;
	// 	m_bCountable = true;
	// }
	else if (strEvent == "SymbolStart")
	{
		for (int i = 0; i < 40; i++)
		{
			CEffectFactory::Create<CHealCircle>("Blue_Circle", L"Blue_Circle", _vec3(51.5f, 25.f, 42.5f));
			CEffectFactory::Create<CHealCircle>("Blue_Circle", L"Blue_Circle", _vec3(57.5f, 25.f, 38.5f));
			CEffectFactory::Create<CHealCircle>("Red_Circle", L"Red_Circle", _vec3(67.5f, 25.f, 38.5f));
			CEffectFactory::Create<CHealCircle>("Blue_Circle", L"Blue_Circle", _vec3(73.5f, 25.f, 42.5f));
		}
		Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(_vec3(51.5f, 25.f, 42.5f), 1.f, BLUE, 12, 6.2f);
		Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(_vec3(57.5f, 25.f, 38.5f), 1.f, BLUE, 12, 6.2f);
		Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(_vec3(67.5f, 25.f, 38.5f), 1.f, RED, 12, 6.2f);
		Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(_vec3(73.5f, 25.f, 42.5f), 1.f, BLUE, 12, 6.2f);

		
	}
	else if (strEvent == "SymbolAttack")
	{
		m_bIsSymbolAttackCycle = true; // 이벤트 간격 늘리기 완료
	}
	else if (strEvent == "Kouku_Hide")
	{
		m_pRootPart->pTrans->m_vInfo[INFO_POS] = _vec3(62.5f, 0.f, 48.7f);
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"smokebomb1_2_sws_bip.wav", L"smokebomb1_sws_bip_1.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);

		// PlayAnimationOnce(&m_arrAnim[REST]);
	}
	else if (strEvent == "Ready_Circle")
	{
		// 더블 해머 1타
		m_vKoukuHammerPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vKoukuHammerPos
			, READY_CIRCLE, CGameUtilMgr::s_vZero, _vec3(4.f, 4.f, 4.f), 35, 35);
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vKoukuHammerPos
			, ATTACK_CIRCLE, CGameUtilMgr::s_vZero, _vec3(4.f, 4.f, 4.f), 100, 35);
		
		
	}
	else if(strEvent == "BasicAttackColl_1")
	{
		m_bIsBasicAttackColl = true; // 완료
	}
	else if (strEvent == "DoubleHammer_1")
	{
		m_bIsDoubleHammerColl_1 = true;

		// 더블 해머 2타
		m_vKoukuHammerPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 5.f;;
		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vKoukuHammerPos
			, READY_CIRCLE, CGameUtilMgr::s_vZero, _vec3(5.f, 5.f, 5.f), 34, 34);

		CEffectFactory::AttackRange_Create("Attack_Range_Circle", L"Attack_Range_Circle", m_vKoukuHammerPos
			, ATTACK_CIRCLE, CGameUtilMgr::s_vZero, _vec3(5.f, 5.f, 5.f), 100, 34);
	
	}
	else if (strEvent == "DoubleHammer_2")
	{
		m_bIsDoubleHammerColl_2 = true;
	}
	else if (strEvent == "HorrorAttack_Start")
	{
		CSoundMgr::GetInstance()->PlaySoundRandomChannel({ L"horror_1.wav", L"horror_2.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS], CHANNELID(23));
		m_bKoukuShadow = true;
	}
	else if (strEvent == "Countable")
	{
		m_bCountable = true;
		Get_GameObject<CFireWork_Kouku>(LAYER_EFFECT, L"Counter_Particle")->Add_Particle(m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.6f, D3DXCOLOR(0.5f, 0.5f, 1.f, 0), 256, 0.4f);

		//파티클 추가
	}
	else if (strEvent == "Countable_End")
	{
		m_bCountable = false;
	}
	
	else if (strEvent == "Horror_Attack")
	{

		
		m_bIsHorrorAttack = true;


		m_fCurTime = 0.f;
	}

	else if (strEvent == "counter_sound")
	{
		// CSoundMgr::GetInstance()->PlaySoundChannel(L"counter_hit5.wav", m_pRootPart->pTrans->m_vInfo[INFO_POS], CHANNELID(23));
	}
	else if (strEvent == "HorrorAttack_End")
	{
		m_bIsHorrorAttack = false;
		m_bKoukuShadow = false;
	}
	else if (strEvent == "Intro_End")
	{
		CSoundMgr::GetInstance()->PlayBGM(L"kouku_bgm_0.wav", 0.6f);
	}
	else if (strEvent == "AnimStopped")
	{
		if (m_eState == STUN)
		{
			m_eState = IDLE;
			m_bStopAnim = false;
		}
		else if (m_eState == DEAD)
		{
			if (m_pBossHPUI)
				m_pBossHPUI->KillHpbar();

			for (int i = 0; i < 5; i++)
			{
				CEffectFactory::Create<CCloud>("Creeper_Cloud", L"Creeper_Cloud", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
			}
			CSoundMgr::GetInstance()->PlaySound(L"koukuSaton_Dead_0.ogg", { 59.5f, 25.f ,35.5f });
			CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
			// CClearUI* pClearUI = CUIFactory::Create<CClearUI>("ClearUI", L"ClearUI", 0, WINCX * 0.5f, WINCY * 0.2f, WINCX* 0.4f, WINCY* 0.4f);
			// pClearUI->SetUITexture(26);
			m_bDelete = true;

			for (auto e: Get_Layer(LAYER_GAMEOBJ)->Get_MapObject())
			{
				if (auto pTrail = dynamic_cast<CSkeletalGhostTrail*>(e.second))
					pTrail->SetDead();
			}

			if (g_bOnline && m_bRemote == false)
			{
				Protocol::C_KOUKU_RESULT pkt;
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(pkt));	
			}
		}
	}

}

_int CKouku::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete)
	{
		return OBJ_DEAD;
	}

	CMonster::Update_Object(fTimeDelta);
	CScene* pCurScene = CManagement::GetInstance()->GetScene();
	if(m_fLightTime > m_fCurLightTime)
	{
		//씬 가져오기
		dynamic_cast<CStage_Kouku*>(pCurScene)->CounterLightColor(0.4f);
	}
	else 
		dynamic_cast<CStage_Kouku*>(pCurScene)->CounterLightColor(0.8f);


	m_fCurLightTime += fTimeDelta;

	m_fCurTrailTime += fTimeDelta;

	if (m_bKoukuShadow && m_eState != DEAD)
	{
		if (m_fCurTrailTime > m_fTrailTime)
		{
			m_fCurTrailTime = 0.f;
			CTransform* pOwnerTrans = this->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			_matrix matWorld;
			CGameUtilMgr::MatWorldComposeEuler(matWorld, { 0.6f, 0.6f, 0.6f }, pOwnerTrans->m_vAngle, pOwnerTrans->m_vInfo[INFO_POS]);
			CObjectFactory::CreateGhostTrail("GhostTrail", L"GhostTrail", this, matWorld)
				->SetColorTime(0.3f, D3DCOLOR_ARGB(180, 200,55 ,0));
		}
	}

	m_pBossHPUI->SetCurHp(m_pStat->GetHP());

	if (m_pCurAnim == m_pIdleAnim) // 이전 애니메이션 종료
		m_bCanPlayAnim = true;



	// if (!m_bIntroPlay && m_bStartPlay)
	// {
	// 	PlayAnimationOnce(&m_arrAnim[INTRO]);
	// 	m_pBossHPUI = CUIFactory::Create<CKoukuHpUI>("KoukuHPUI", L"KoukuHPUI", -1, WINCX * 0.5f, WINCY * 0.15f, 500, 25);
	// 	m_pBossHPUI->SetOwner(L"쿠 크", this, m_pStat->GetMaxHP());
	// 	m_bIntroPlay = true;
	// }

	// 상태 변경 조건 설정
		StateChange();

	// 각 상태에 따른 프레임 마다 실행할 함수 지정
	switch (m_eState)
	{
	case INTRO:
		m_strState = "INTRO";
		break;
	case STUN:
		break;
	case WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		m_strState = "WALK";
		break;
	case HAMMER_IN:
		break;
	case HAMMER_OUT:
		break;
	case HAMMER_ATTACK:
		m_strState = "HAMMER_ATTACK";
	break;
	case HORROR_ATTACK:
		m_fCurTime += fTimeDelta;

		if (m_fCurTime <= m_fTime)
		{
			m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 4.f * fTimeDelta;
		}
		m_strState = "HORROR_ATTACK";
		break;
	case BASIC_ATTACK:
		m_strState = "BASICATTACK";
		break;

	case SYMBOL_HIDE:
		m_strState = "SYMBOL_HIDE";
		break;

	case DEAD:
		m_pBossHPUI->KillHpbar();
		break;
	default:
		break;
	}


	if (m_eState != HORROR_ATTACK)
	{
		// if (m_fCurTime > m_fTime)
			// m_fCurTime = 0.f;
	}

	

	return OBJ_NOEVENT;
}

void CKouku::LateUpdate_Object()
{
	CMonster::LateUpdate_Object();

	if (m_bIsSymbolGimmick)
	{
		set<CGameObject*> setPlayer_1;
		_vec3 Left_BludCircle_1 = _vec3(51.5f, 25.f, 42.5f);
		Engine::GetOverlappedObject(setPlayer_1, Left_BludCircle_1, 2.f);

		for (auto& obj : setPlayer_1)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, _vec3(51.5f, 25.f, 42.5f), this, DT_KOUKU_SYMBOL_BLUE);
		}

		DEBUG_SPHERE(Left_BludCircle_1, 10.f, 6.2f);
		IM_LOG("Create Left_Blue_1 Circle");


		set<CGameObject*> setPlayer_2;
		_vec3 Left_BludCircle_2 = _vec3(57.5f, 25.f, 38.5f);
		Engine::GetOverlappedObject(setPlayer_2, Left_BludCircle_2, 2.f);

		for (auto& obj : setPlayer_2)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, _vec3(57.5f, 25.f, 38.5f), this, DT_KOUKU_SYMBOL_BLUE);
		}

		DEBUG_SPHERE(Left_BludCircle_2, 10.f, 6.2f);
		IM_LOG("Create Left_Blue_2 Circle");


		set<CGameObject*> setPlayer_3;
		_vec3 Right_RedCircle_3 = _vec3(67.5f, 25.f, 38.5f);
		Engine::GetOverlappedObject(setPlayer_3, Right_RedCircle_3, 2.f);


		m_iRedSymbolCnt = 0;
		for (auto& obj : setPlayer_3)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, _vec3(67.5f, 25.f, 38.5f), this, DT_KOUKU_SYMBOL_RED);

			m_iRedSymbolCnt++;
		}

		DEBUG_SPHERE(Right_RedCircle_3, 10.f, 0.1f);
		IM_LOG("Create Right_Red_3 Circle");

		set<CGameObject*> setPlayer_4;
		_vec3 Left_BlueCircle_4 = _vec3(73.5f, 25.f, 42.5f);
		Engine::GetOverlappedObject(setPlayer_4, Left_BlueCircle_4, 2.f);

		for (auto& obj : setPlayer_4)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, _vec3(73.5f, 25.f, 42.5f), this, DT_KOUKU_SYMBOL_BLUE);
		}

		DEBUG_SPHERE(Left_BlueCircle_4, 10.f, 6.2f);
		IM_LOG("Create Right_Blue_4 Circle");


		if (m_bIsSymbolAttackCycle)
		{
			set<CGameObject*> setPlayer;
			_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];

			Engine::GetOverlappedObject(setPlayer, KoukuPos, 100.f);

			for (auto& obj : setPlayer)
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				{
					CStatComponent* PlayerStatCom = pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC);

					if (PlayerStatCom->IsSatonSymbol_Blue() == false && m_iRedSymbolCnt != 2)
					{
						PlayerStatCom->TakeDamage(20, pPlayer->GetInfo(INFO_POS), this, DT_END);
					}
				}
			}
			m_bIsSymbolAttackCycle = false;
		}
	}

	if(m_bIsBasicAttackColl)
	{
		set<CGameObject*> Player;
		_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		_vec3 FromPos = KoukuPos + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2;
		Engine::GetOverlappedObject(Player, KoukuPos, 3.f);

		for (auto& obj : Player)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(15, FromPos, this, DT_KNOCK_BACK);
				CSoundMgr::GetInstance()->PlaySoundRandom({ L"attack1_hit.wav", L"attack1_hit_2.wav" }, { 59.5f, 25.f ,35.5f });

			}
		}
		m_bIsBasicAttackColl = false;
	}

	if (m_bIsDoubleHammerColl_1)
	{
		set<CGameObject*> Player;
		_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		// _vec3 FromPos = KoukuPos + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2;
		Engine::GetOverlappedObject(Player, KoukuPos, 4.f);

		for (auto& obj : Player)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(20, KoukuPos, this, DT_KNOCK_BACK);
		}
		CEffectFactory::Create<CUVCircle>("Kouku_Explosion", L"Kouku_Explosion",
			_vec3(m_vKoukuHammerPos.x, m_vKoukuHammerPos.y + 0.2f, m_vKoukuHammerPos.z));
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"attack1_hit.wav", L"attack1_hit_2.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		m_bIsDoubleHammerColl_1 = false;
	}

	if (m_bIsDoubleHammerColl_2)
	{
		set<CGameObject*> Player;
		_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		_vec3 FromPos = KoukuPos + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 5;
		Engine::GetOverlappedObject(Player, KoukuPos, 5.f);

		for (auto& obj : Player)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(20, FromPos, this, DT_KNOCK_BACK);
		}
		CEffectFactory::Create<CUVCircle>("Hammer2_Explosion", L"Hammer2_Explosion",
			_vec3(m_vKoukuHammerPos.x, m_vKoukuHammerPos.y + 0.2f, m_vKoukuHammerPos.z));
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"attack1_hit.wav", L"attack1_hit_2.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		m_bIsDoubleHammerColl_2 = false;
	}

	if(m_bIsHorrorAttack)
	{
		set<CGameObject*> Player;
		_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		// _vec3 FromPos = KoukuPos + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 5;
		Engine::GetOverlappedObject(Player, KoukuPos, 2.5f);

		for (auto& obj : Player)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(5, KoukuPos, this, DT_KNOCK_BACK);
		}
		// DEBUG_SPHERE(KoukuPos, 2.5f, 0.1f);

		// m_bIsHorrorAttack = false;
	}

	// if (!m_pStat->IsStun())
	// {
	// 	if (m_bCountable)
	// 	{
	// 		set<CGameObject*> Player;
	// 		_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 1.5f;
	// 		Engine::GetOverlappedObject(Player, KoukuPos, 2.5f);
	//
	// 		for (auto& obj : Player)
	// 		{
	// 			if (CWeapon* pWeapon = dynamic_cast<CWeapon*>(obj))
	// 				m_pStat->TakeDamage(0, KoukuPos, this, DT_STUN);
	// 			m_bCountable = false;
	// 		}
	// 	}
	// }
}

void CKouku::Kouku_Stun_Success(_uint iID)
{
	_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];

	if(!m_pStat->IsStun())
	{
		if (m_bCountable)
		{
			// m_pStat->TakeDamage(0, KoukuPos, this, DT_STUN);
			// Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			// 	->PlayShake(0.1f, 1.f);
			SetKoukuCounter();

			if (g_bOnline)
			{
				Protocol::C_KOUKU_COUNTER counterPkt;
				counterPkt.mutable_player()->set_id(iID);
				CClientServiceMgr::GetInstance()->Broadcast(ServerPacketHandler::MakeSendBuffer(counterPkt));	
			}

			m_bCountable = false;
		}
	}
	DEBUG_SPHERE(KoukuPos, 1.f, 0.1f);

}

void CKouku::SetKoukuCounter()
{
	const _vec3& KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
	m_pStat->TakeDamage(0, KoukuPos, this, DT_STUN);
	Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
		->PlayShake(0.15f, 1.f);
	CSoundMgr::GetInstance()->PlaySoundChannel(L"grogi_edit_2_2.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS], CHANNELID(23),1.f);
	m_bKoukuShadow = false;

	m_fCurLightTime = 0.f;
	m_bCountable = false;
}

void CKouku::Free()
{
	CMonster::Free();
}

CKouku* CKouku::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote)
{
	CKouku* pInstance = new CKouku(pGraphicDev);
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

void CKouku::StateChange()
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

	if (m_pStat->IsStun())
	{
		if (m_bReserveStop == false)
		{
			m_eState = STUN;
			PlayAnimationOnce(&m_arrAnim[STUN], true);
			m_bCanPlayAnim = false;
			m_bMove = false;
			return;
		}
	}
	if (m_bIntroPlay)
	{
		m_eState = INTRO;
		_vec3 vLook{ _vec3(62.5f, 25.f, 33.5f) };
		D3DXVec3Normalize(&vLook, &vLook);
		m_pRootPart->pTrans->m_vInfo[INFO_LOOK] = m_vTargetPos;

		PlayAnimationOnce(&m_arrAnim[INTRO]);
		m_bIntroPlay = false;
		m_bCanPlayAnim = false;
		m_bMove = false;
		SetOff();
		return;
	}

	if (m_bBasicAttack && m_bCanPlayAnim)
	{
		m_eState = BASIC_ATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[BASIC_ATTACK]);
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"attack1_1.wav", L"attack1_2.wav" ,L"attack1_3.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		SetOff();
		return;
	}

	if (m_bDoubleHammer && m_bCanPlayAnim)
	{
		m_eState = HAMMER_ATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[HAMMER_ATTACK]);
		CSoundMgr::GetInstance()->PlaySoundRandom({ L"attack2_1.wav", L"attack2_2.wav" }, m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		SetOff();
		return;
	}

	if (m_bHorrorAttack && m_bCanPlayAnim)
	{
		m_eState = HORROR_ATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[HORROR_ATTACK]);
		m_bMove = false;
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}

	if (m_bKoukuSymbol && m_bCanPlayAnim)
	{
		m_eState = SYMBOL_HIDE;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SYMBOL_HIDE]);
		m_bMove = false;
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}
	//
	if (m_bMove && m_bCanPlayAnim)
	{
		m_eState = WALK;
		RotateToTargetPos(m_vTargetPos);
		m_pIdleAnim = &m_arrAnim[WALK];
		m_pCurAnim = &m_arrAnim[WALK];
		return;
	}
	// if (m_bCanPlayAnim)
	// {
	// 	m_eState = WALK;
	// 	RotateToTargetPos(m_vTargetPos);
	// 	m_pIdleAnim = &m_arrAnim[WALK];
	// 	m_pCurAnim = &m_arrAnim[WALK];
	// 	return;
	// }
	

	if (m_bCanPlayAnim)
	{
		m_eState = IDLE;
		// RotateToTargetPos(m_vTargetPos);
		m_pIdleAnim = &m_arrAnim[IDLE];
		m_pCurAnim = &m_arrAnim[IDLE];
		return;
	}
}

void CKouku::Symbol_Attack()
{
	set<CGameObject*> setPlayer;
	_vec3 KoukuPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];

	Engine::GetOverlappedObject(setPlayer, KoukuPos, 10.f);

	for (auto& obj : setPlayer)
	{

		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
		{
			CStatComponent* PlayerStatCom = pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC);

			if (!PlayerStatCom->IsSatonSymbol_Blue())
			{
				PlayerStatCom->TakeDamage(20, pPlayer->GetInfo(INFO_POS), this, DT_KOUKU_SYMBOL_BLUE);
			}
		}
	}
}


