#include "stdafx.h"
#include "..\Header\Enderman.h"
#include "StatComponent.h"
#include "EndermanController.h"
#include "BossHPUI.h"
#include "AbstFactory.h"
#include "EndermanTrail.h"

CEnderman::CEnderman(LPDIRECT3DDEVICE9 pGraphicDev) : CMonster(pGraphicDev)
{
}

CEnderman::CEnderman(const CMonster & rhs) : CMonster(rhs)
{
}


CEnderman::~CEnderman()
{
}

HRESULT CEnderman::Ready_Object()
{
	CMonster::Ready_Object();

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/idle.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/walk.anim");	
	m_arrAnim[ANIM_CHOP] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/chop.anim");
	m_arrAnim[ANIM_SMASH] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/smash.anim");
	m_arrAnim[ANIM_ARMATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/armattack.anim");
	m_arrAnim[ANIM_PISTOL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/pistol.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Enderman/dead.anim");

	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 3.f;

	m_pStat->SetMaxHP(2000);

	m_pStat->SetHurtSound({
		L"sfx_mob_endermanHit-001.ogg",
		L"sfx_mob_endermanHit-002.ogg" ,
		L"sfx_mob_endermanHit-003.ogg" ,
		L"sfx_mob_endermanHit-004.ogg" });

	
	CController* pController = Add_Component<CEndermanController>(L"Proto_EndermanController", L"Proto_EndermanController", ID_DYNAMIC);
	pController->SetOwner(this);

	m_pBossHPUI = CUIFactory::Create<CBossHPUI>("BossHPUI", L"BossHPUI", -1, WINCX * 0.5f, WINCY * 0.15f, 500, 25);
	m_pBossHPUI->SetOwner(L"엔더맨", this, m_pStat->GetMaxHP());
	m_pBossHPUI->SetNamePos(0.47f);

	m_bCantCC = true;

	return S_OK;
}

void CEnderman::AnimationEvent(const string & strEvent)
{
	if (strEvent == "AttackFire")
	{
		m_bCreateTrail = false;
		m_bAttackFire = true;
	}
	else if (strEvent == "PistolStart")
	{
		m_bPistolStart = true;
	}
	else if (strEvent == "PistolFire")
	{
		m_bPistolStart = false;
	}
	else if (strEvent == "ArmAttack")
	{
		m_bArmAttakcFire = true;
	}
	else if (strEvent == "ActionEnd")
	{
		m_bCanPlayAnim = true;
	}
	else if (strEvent == "AnimStopped")
	{
		CSoundMgr::GetInstance()->StopAll();
		m_pBossHPUI->KillHpbar();
		m_bDelete = true;
	}
	else if (strEvent == "Step")
	{
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_endermanIdle-001.ogg",
			L"sfx_mob_endermanIdle-002.ogg",
			L"sfx_mob_endermanIdle-003.ogg"},
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.3f);
	}
	else if (strEvent == "CreateShader")
	{
		m_bCreateTrail = true;
	}
}

_int CEnderman::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	m_fCurPistolTime += fTimeDelta;
	m_fCurClockingCollTime += fTimeDelta;

	//clockingoff
	if (m_bClocking && m_fCurClockingCollTime > m_fClockingCollTime)
	{
		m_bClocking = false;
		D3DXVec3Normalize(&m_vBeforTargetLook, &m_vBeforTargetLook);
		m_pRootPart->pTrans->m_vInfo[INFO_POS] = m_vTargetPos + m_vBeforTargetLook * 3.f;
		m_pBossHPUI->SetRender(true);
	}

	//clockingOn
	if (m_bClocking && m_eState != SMASH &&m_eState != CHOP &&m_eState != ARMATTACK && m_eState != PISTOL)
	{
		m_pBossHPUI->SetRender(false);
		return OBJ_NOEVENT;
	}

	
	CMonster::Update_Object(fTimeDelta);
	m_pBossHPUI->SetCurHp(m_pStat->GetHP());

	


	if (m_pCurAnim == m_pIdleAnim) // 이전 애니메이션 종료
		m_bCanPlayAnim = true;

	// 상태 변경 조건 설정
	StateChange();

	// 각 상태에 따른 프레임 마다 실행할 함수 지정
	switch (m_eState)
	{
	case IDLE:
		break;
	case WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		break;
	case CHOP:
		break;
	case SMASH:
		break;
	case CLOCKING:
		break;
	case ARMATTACK:
		break;
	case PISTOL:
		break;
	case STUN:
		break;
	case DEAD:
		break;
	default:
		break;
	}

	return OBJ_NOEVENT;
}

void CEnderman::LateUpdate_Object()
{
	CMonster::LateUpdate_Object();

	if (m_bCreateTrail)
		CreateTrail({ "body","head", "leg_l", "leg_r" });
	

	if (m_bAttackFire)
	{

		set<CGameObject*> setObj;
		_vec3 vAttackPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + (m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.f);
		Engine::GetOverlappedObject(setObj, vAttackPos, 2.5f);

		for (auto& obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(50, m_pRootPart->pTrans->m_vInfo[INFO_POS], this);
		}
	
	
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_endermanSwing-001.ogg",
			L"sfx_mob_endermanSwing-002.ogg",
			L"sfx_mob_endermanSwing-003.ogg"}, vAttackPos, 0.2f);
		m_bAttackFire = false;
	}

	
	if (m_bArmAttakcFire)
	{
		_vec3 vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = m_pRootPart->pTrans->m_vInfo[INFO_LOOK];

		for (_int i = 1; i < 4; ++i)
		{
			set<CGameObject*> setObj;
			_vec3 vAttackPos = vPos + vLook * 2.f * _float(i);
			Engine::GetOverlappedObject(setObj, vAttackPos, 1.f);

			for (auto& obj : setObj)
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
					pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(80, m_pRootPart->pTrans->m_vInfo[INFO_POS], this);
			}
		}
		


		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_endermanTeleportVoiceHigh-001.ogg",
			L"sfx_mob_endermanTeleportVoiceHigh-002.ogg",
			L"sfx_mob_endermanTeleportVoiceHigh-003.ogg"},
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.2f);
		m_bArmAttakcFire = false;
	}

	if (m_bPistolStart && m_fCurPistolTime > m_fPistolFreq)
	{
		_vec3 vPos = m_pRootPart->pTrans->m_vInfo[INFO_POS];
		_vec3 vLook = m_pRootPart->pTrans->m_vInfo[INFO_LOOK];

		for (_int i = 1; i < 4; ++i)
		{
			set<CGameObject*> setObj;
			_vec3 vAttackPos = vPos + vLook * 2.f * _float(i);
			Engine::GetOverlappedObject(setObj, vAttackPos, 1.5f);

			for (auto& obj : setObj)
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
					pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(10, m_pRootPart->pTrans->m_vInfo[INFO_POS], this, DT_STIFFEN);
			}
		}



		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_endermanTeleportVoiceHigh-001.ogg",
			L"sfx_mob_endermanTeleportVoiceHigh-002.ogg",
			L"sfx_mob_endermanTeleportVoiceHigh-003.ogg" },
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.2f);


		m_fCurPistolTime = 0.f;
	}

}




void CEnderman::Free()
{
	CMonster::Free();
}

CEnderman * CEnderman::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring & wstrPath, _bool bRemote)
{
	CEnderman* pInstance = new CEnderman(pGraphicDev);
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

void CEnderman::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"sfx_mob_endermanDeath-001.ogg", m_pRootPart->pTrans->m_vInfo[INFO_POS]);
			m_eState = DEAD;
			PlayAnimationOnce(&m_arrAnim[ANIM_DEAD], true);
			m_bChop = false;
			m_bSmash = false;
			m_bMove = false;
			m_bCanPlayAnim = false;
			m_pColl->SetStop();
		}
		return;
	}

	if (m_pStat->IsStun())
	{
		m_eState = STUN;
		m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
		m_pCurAnim = &m_arrAnim[ANIM_IDLE];
		m_bChop = false;
		m_bSmash = false;
		m_bMove = false;

		return;
	}

	if (m_bSmash && m_bCanPlayAnim)
	{
		m_eState = SMASH;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[ANIM_SMASH]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		m_bSmash = false;
		return;
	}

	if (m_bChop && m_bCanPlayAnim)
	{
		m_eState = CHOP;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[ANIM_CHOP]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		m_bChop = false;
		return;
	}

	if (m_bPistol && m_bCanPlayAnim)
	{
		m_eState = PISTOL;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[ANIM_PISTOL]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		m_bPistol = false;
		return;
	}

	if (m_bArmAttack && m_bCanPlayAnim)
	{
		m_eState = ARMATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[ANIM_ARMATTACK]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		m_bArmAttack = false;
		return;
	}

	if (m_bMove && m_bCanPlayAnim)
	{
		m_eState = WALK;
		RotateToTargetPos(m_vTargetPos);
		m_pIdleAnim = &m_arrAnim[ANIM_WALK];
		m_pCurAnim = &m_arrAnim[ANIM_WALK];
		return;
	}

	if (m_bCanPlayAnim)
	{
		m_eState = IDLE;
		m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
		m_pCurAnim = &m_arrAnim[ANIM_IDLE];
		return;
	}
}

void CEnderman::CreateTrail(const vector<string>& vecExcludeParts)
{
	//생성
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK(pLayer);

	CGameObject* pGameObject = nullptr;
	pGameObject = CEndermanTrail::Create(m_pGraphicDev, this, vecExcludeParts);
	NULL_CHECK(pGameObject);
	Engine::AddGameObject(LAYER_GAMEOBJ, L"Proto_EndermanTrailShaderCom", pGameObject);
	dynamic_cast<CEndermanTrail*>(pGameObject)->SetColorTime(1.f, D3DCOLOR_ARGB(200, 255, 051, 255));
	Engine::CTransform* pTrans = pGameObject->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
	_matrix matWorld = m_pRootPart->pTrans->m_matWorld;
	matWorld.m[0][0] *= 0.7f;
	matWorld.m[1][1] *= 0.7f;
	matWorld.m[2][2] *= 0.7f;
	pTrans->Set_WorldDecompose(matWorld);
	//
}
