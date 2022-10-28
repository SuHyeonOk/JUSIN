#include "stdafx.h"
#include "..\Header\Zombie.h"
#include "StatComponent.h"
#include "ZombieController.h"

CZombie::CZombie(LPDIRECT3DDEVICE9 pGraphicDev): CMonster(pGraphicDev)
{
}

CZombie::CZombie(const CMonster& rhs): CMonster(rhs)
{
}

CZombie::~CZombie()
{
}

HRESULT CZombie::Ready_Object()
{
	CMonster::Ready_Object();

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Zombie/idle.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Zombie/walk.anim");
	if (rand() % 2 == 0)
		m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Zombie/dead_a.anim");
	else
		m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Zombie/dead_b.anim");
	m_arrAnim[ANIM_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Zombie/attack.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 2.f;

	m_pStat->SetMaxHP(100);

	m_pStat->SetHurtSound({
		L"DLC_sfx_mob_monster_Hurt-001.ogg",
		L"DLC_sfx_mob_monster_Hurt-002.ogg" ,
		L"DLC_sfx_mob_monster_Hurt-003.ogg" });

	if (m_bRemote)
	{
		CController* pController = Add_Component<CZombieController>(L"Proto_ZombieRemoteController", L"Proto_ZombieRemoteController", ID_DYNAMIC);
		pController->SetOwner(this);
	}
	else
	{
		CController* pController = Add_Component<CZombieController>(L"Proto_ZombieController", L"Proto_ZombieController", ID_DYNAMIC);
		pController->SetOwner(this);
	}


	return S_OK;
}

void CZombie::AnimationEvent(const string& strEvent)
{
	if (strEvent == "AttackFire")
	{
		m_bAttackFire = true;
	}
	else if (strEvent == "ActionEnd")
	{
		m_bCanPlayAnim = true;
	}
	else if (strEvent == "AnimStopped")
	{
		m_bDelete = true;
	}
	else if (strEvent == "Step")
	{
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_zombieStepGeneric-001.ogg",
			L"sfx_mob_zombieStepGeneric-002.ogg",
			L"sfx_mob_zombieStepGeneric-003.ogg",
			L"sfx_mob_zombieStepGeneric-004.ogg",
			L"sfx_mob_zombieStepGeneric-005.ogg", },
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.3f);
	}
}

_int CZombie::Update_Object(const _float& fTimeDelta)
{


	if (m_bDelete)
	{

		return OBJ_DEAD;
	}

	CMonster::Update_Object(fTimeDelta);



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
	case ATTACK:
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

void CZombie::LateUpdate_Object()
{
	CMonster::LateUpdate_Object();

	if (m_bAttackFire)
	{
		set<CGameObject*> setObj;
		_vec3 vAttackPos = m_pRootPart->pTrans->m_vInfo[INFO_POS] + (m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * 2.f);
		Engine::GetOverlappedObject(setObj, vAttackPos, 1.f);
	
		for (auto& obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				       ->TakeDamage(20, m_pRootPart->pTrans->m_vInfo[INFO_POS], this);
		}
		DEBUG_SPHERE(vAttackPos, 1.f, 1.f);
		IM_LOG("Fire");
	
		CSoundMgr::GetInstance()->PlaySoundRandom({ 
			L"sfx_mob_zombieAttack-001.ogg", 
			L"sfx_mob_zombieAttack-002.ogg",
			L"sfx_mob_zombieAttack-003.ogg",
			L"sfx_mob_zombieAttack-004.ogg"}, vAttackPos, 0.2f);
		m_bAttackFire = false;
	}
}

void CZombie::Free()
{
	CMonster::Free();
}

CZombie* CZombie::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath, _bool bRemote)
{
	CZombie* pInstance = new CZombie(pGraphicDev);
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

void CZombie::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			CSoundMgr::GetInstance()->PlaySoundRandom({
				L"sfx_mob_zombieDeath-001.ogg",
				L"sfx_mob_zombieDeath-002.ogg",
				L"sfx_mob_zombieDeath-003.ogg" },
				m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.5f);

			m_eState = DEAD;
			PlayAnimationOnce(&m_arrAnim[ANIM_DEAD], true);
			m_bAttack = false;
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
		m_bAttack = false;
		m_bMove = false;
		StopCurAnimation();
		
		return;
	}


	if (m_bAttack && m_bCanPlayAnim)
	{
		m_eState = ATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK]);
		m_bCanPlayAnim = false;
		m_bMove = false;
		m_bAttack = false;
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