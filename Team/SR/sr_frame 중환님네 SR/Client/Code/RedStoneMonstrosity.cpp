#include "stdafx.h"
#include "..\Header\RedStoneMonstrosity.h"
#include "StatComponent.h"
#include "RedStoneMonstrosityController.h"
#include "AbstFactory.h"
#include "RedStoneCube.h"
#include "RedStoneMonstrosityBullet.h"
#include "Particle.h"
#include "SphereEffect.h"
#include "StaticCamera.h"
#include "BossHPUI.h"
#include "ClearUI.h"
CRedStoneMonstrosity::CRedStoneMonstrosity(LPDIRECT3DDEVICE9 pGraphicDev) : CMonster(pGraphicDev)
{
}

CRedStoneMonstrosity::CRedStoneMonstrosity(const CMonster& rhs) : CMonster(rhs)
{
}

CRedStoneMonstrosity::~CRedStoneMonstrosity()
{
}

HRESULT CRedStoneMonstrosity::Ready_Object()
{
	CMonster::Ready_Object();
	
	m_arrAnim[INTRO] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/intro.anim");
	m_arrAnim[WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/walk.anim");
	m_arrAnim[DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/dead.anim");
	m_arrAnim[CHOP] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/chop.anim");
	m_arrAnim[SPIT] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/spit.anim");
	m_arrAnim[WINDMILL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/windmill.anim");
	m_arrAnim[SUMMON] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneMonstrosity/summon.anim");

	m_pIdleAnim = &m_arrAnim[WALK];
	m_pCurAnim = &m_arrAnim[INTRO];
	m_eState = INTRO;
	m_fSpeed = 1.5f;

	m_pStat->SetMaxHP(3000);

	m_pStat->SetHurtSound({
		L"sfx_mob_redstoneGolemHurt-001.ogg",
		L"sfx_mob_redstoneGolemHurt-002.ogg",
		L"sfx_mob_redstoneGolemHurt-003.ogg",
		L"sfx_mob_redstoneGolemHurt-004.ogg"});

	CController* pController = Add_Component<CRedStoneMonstrosityController>(L"Proto_RedStoneMonstrosityController", L"Proto_RedStoneMonstrosityController", ID_DYNAMIC);
	pController->SetOwner(this);

	m_pBossHPUI = CUIFactory::Create<CBossHPUI>("BossHPUI", L"BossHPUI", -1, WINCX * 0.5f, WINCY * 0.15f, 500, 25);
	m_pBossHPUI->SetOwner(L"레드 스톤 몬스터", this, m_pStat->GetMaxHP());

	//cc면역
	m_bCantCC = true;
	m_bCanPlayAnim = false;

	return S_OK;
}

void CRedStoneMonstrosity::AnimationEvent(const string& strEvent)
{
	if (strEvent == "ChopFire")
	{
		CEffectFactory::Create<CSphereEffect>("Golem_Melee_Shpere_L", L"Golem_Melee_Shpere_L");
		CEffectFactory::Create<CSphereEffect>("Golem_Melee_Shpere_M", L"Golem_Melee_Shpere_M");

		CEffectFactory::Create<CSphereEffect>("Golem_Melee_L", L"Golem_Melee_L");
		CEffectFactory::Create<CSphereEffect>("Golem_Melee_M", L"Golem_Melee_M");
		CEffectFactory::Create<CSphereEffect>("Golem_Melee_S", L"Golem_Melee_S");
		for (int i = 0; i < 15; i++)
		{
			CEffectFactory::Create<CCloud>("Golem_Cloud", L"Golem_Cloud", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		}
		//완전히 찍을 때
		Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pRootPart->pTrans->m_vInfo[INFO_POS], 3.f, D3DXCOLOR(0.88f, 0.35f, 0.24f, 1.0f), 12, 0.8f);

		//카메라 쉐이킹
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayShake(0.2f, 0.8f);


		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_redstoneGolemChop-001.ogg",
			L"sfx_mob_redstoneGolemChop-002.ogg", 
			L"sfx_mob_redstoneGolemChop-003.ogg"},
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.7f);

		m_bChopFire = true;
	}
	else if (strEvent == "ActionEnd")
	{
		m_bCanPlayAnim = true;
	}
	else if (strEvent == "AnimStopped")
	{
		if (m_pBossHPUI)
			m_pBossHPUI->KillHpbar();

		CSoundMgr::GetInstance()->StopAll();
		CClearUI* pClearUI = CUIFactory::Create<CClearUI>("ClearUI", L"ClearUI", 0, WINCX * 0.5f, WINCY * 0.2f, WINCX* 0.4f, WINCY* 0.4f);
		pClearUI->SetUITexture(26);

		m_bDelete = true;
	}
	else if (strEvent == "SpitFire")
	{

		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_redstoneGolemSpit-001.ogg",
			L"sfx_mob_redstoneGolemSpit-002.ogg",
			L"sfx_mob_redstoneGolemSpit-003.ogg" },
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.6f);

		_matrix matWorld;
		
		auto& itr = m_mapParts.find("head");
		if (itr == m_mapParts.end())
			return;

		_matrix mat = itr->second->GetWorldMat();
		_vec3 vPos = _vec3(mat._41 , mat._42, mat._43);
		_vec3 vLook = m_vTargetPos - vPos;
		D3DXVec3Normalize(&vLook, &vLook);

		for (_int i = -3; i <= 3; ++i)
		{			
			vLook.x = cosf(D3DXToRadian(10 * i)) * vLook.x - sinf(D3DXToRadian(10 * i)) * vLook.z;
			CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, vPos);
			CRedStoneMonstrosityBullet* pMonsterBullet = CObjectFactory::Create<CRedStoneMonstrosityBullet>("RedStoneMonstrosityBullet", L"RedStoneMonstrosityBullet", matWorld);
			pMonsterBullet->SetBulletInform(vLook, _float(rand() % 3 + 9));
		}		
	}
	//소환 모션
	else if (strEvent == "Sommon_End")
	{
		for (_int i = 0; i < 6; ++i)
		{
			m_matSommonWorld[i];
			_vec3 vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
			_int randomx = rand() % 20 - 10;
			_int randomz = rand() % 20 - 10;
			CGameUtilMgr::MatWorldComposeEuler(m_matSommonWorld[i], { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { vPos.x + (_float)randomx, vPos.y , vPos.z + (_float)randomz });
			CEffectFactory::Create<CCrack>("Red_Cube_Crack", L"Red_Cube_Crack", m_matSommonWorld[i]);

			CSoundMgr::GetInstance()->PlaySound(L"sfx_prop_redstoneSommon-01.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.2f);
		}
	}
	//몬스터 소환할때 모션
	else if (strEvent == "Motion_End")
	{
		for (_int i = 0; i < 6; ++i)
		{
			CEnemyFactory::Create<CRedStoneCube>("RedStoneCube", L"RedStoneCube", m_matSommonWorld[i]);
		}
	}
	// 윈드밀때 파티클 생성
	else if (strEvent == "Particle_Create")
	{
		for (int j = 0; j < 10; j++)
		{
			CEffectFactory::Create<CCloud>("Golem_Windmill", L"Golem_Windmill", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		}

		CEffectFactory::Create<CUVCircle>("Golem_Circle", L"Golem_Circle", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
		m_bWindmillFire = true;
		m_dwWindTime = GetTickCount64();
	}
	else if (strEvent == "Windmill_Fire")
	{
		m_bWindmillFire = false;
	}
	else if (strEvent == "Step")
	{
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_redstoneGolemStepHeavy-001.ogg",
			L"sfx_mob_redstoneGolemStepHeavy-002.ogg",
			L"sfx_mob_redstoneGolemStepHeavy-003.ogg",
			L"sfx_mob_redstoneGolemStepHeavy-004.ogg" },
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.5f);
	}

}

_int CRedStoneMonstrosity::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	CMonster::Update_Object(fTimeDelta);

	if(m_pBossHPUI)
		m_pBossHPUI->SetCurHp(m_pStat->GetHP());


	if (!m_bIntroPlay)
	{
		PlayAnimationOnce(&m_arrAnim[INTRO]);
		m_bIntroPlay = true;
	}


	if (m_pCurAnim == m_pIdleAnim) // 이전 애니메이션 종료
		m_bCanPlayAnim = true;

	// 상태 변경 조건 설정
	StateChange();

	// 각 상태에 따른 프레임 마다 실행할 함수 지정
	switch (m_eState)
	{
	case INTRO:
		break;
	case WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		break;
	case CHOP:
		break;
	case SPIT:
		break;
	case SUMMON:
		break;
	case WINDMILL:
		break;
	case DEAD:
		break;
	default:
		break;
	}


	return OBJ_NOEVENT;
}

void CRedStoneMonstrosity::LateUpdate_Object()
{
	CMonster::LateUpdate_Object();

	if (m_bChopFire)
	{
		set<CGameObject*> setObj;
		_vec3 vAttackPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + (m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.f);
		Engine::GetOverlappedObject(setObj, vAttackPos, 6.f);

		for (auto& obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(50, m_pRootPart->pTrans->m_vInfo[INFO_POS], this, DT_KNOCK_BACK);
		}
		DEBUG_SPHERE(vAttackPos, 6.f, 1.f);
		IM_LOG("Fire");

		m_bChopFire = false;
	}

	if (m_bWindmillFire)
	{
		if (m_dwWindTime + 500 < GetTickCount64())
		{
			set<CGameObject*> setObj;
			_vec3 vAttackPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + (m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.f);
			Engine::GetOverlappedObject(setObj, vAttackPos, 7.f);

			for (auto& obj : setObj)
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
					pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(25, m_pRootPart->pTrans->m_vInfo[INFO_POS], this, DT_KNOCK_BACK);
			}
			DEBUG_SPHERE(vAttackPos, 7.f, 1.f);

			CSoundMgr::GetInstance()->PlaySound(L"sfx_mob_redstonegolemWindmill-001.ogg",
				m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.5f);

			m_dwWindTime = GetTickCount64();
		}
	
	}
}

void CRedStoneMonstrosity::Free()
{
	CMonster::Free();
}

CRedStoneMonstrosity* CRedStoneMonstrosity::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	CRedStoneMonstrosity* pInstance = new CRedStoneMonstrosity(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CRedStoneMonstrosity::StateChange()
{
	if (m_pStat->IsDead() && m_bReserveStop == false)
	{
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_redstoneGolemDeathHeavy-001.ogg",
			L"sfx_mob_redstoneGolemDeathHeavy-002.ogg",
			L"sfx_mob_redstoneGolemDeathHeavy-003.ogg" },
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.6f);

		m_eState = DEAD;
		PlayAnimationOnce(&m_arrAnim[DEAD], true);
		m_bCanPlayAnim = false;
		return;
	}

	

	if (m_bChop && m_bCanPlayAnim)
	{
		m_eState = CHOP;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[CHOP]);
		m_bCanPlayAnim = false;
		SetOff();	
		return;
	}

	if (m_bSpit && m_bCanPlayAnim)
	{
		m_eState = SPIT;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SPIT]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}

	if (m_bSummon && m_bCanPlayAnim)
	{
		m_eState = SUMMON;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[SUMMON]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}

	if (m_bWindmill && m_bCanPlayAnim)
	{
		m_eState = WINDMILL;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[WINDMILL]);
		m_bCanPlayAnim = false;
		SetOff();
		return;
	}


	if (m_bCanPlayAnim)
	{
		m_eState = WALK;
		RotateToTargetPos(m_vTargetPos);
		m_pIdleAnim = &m_arrAnim[WALK];
		m_pCurAnim = &m_arrAnim[WALK];
		return;
	}

}
