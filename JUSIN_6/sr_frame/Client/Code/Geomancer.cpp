#include "stdafx.h"
#include "..\Header\Geomancer.h"

#include "AbstFactory.h"
#include "StatComponent.h"
#include "GeomancerController.h"
#include "GeomancerWall.h"

CGeomancer::CGeomancer(LPDIRECT3DDEVICE9 pGraphicDev): CMonster(pGraphicDev)
{
}

CGeomancer::CGeomancer(const CMonster& rhs): CMonster(rhs)
{
}

CGeomancer::~CGeomancer()
{
}

HRESULT CGeomancer::Ready_Object()
{
	CMonster::Ready_Object();

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Geomancer/idle.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Geomancer/walk.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Geomancer/dead.anim");
	m_arrAnim[ANIM_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Geomancer/attack.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 3.f;

	m_pStat->SetHurtSound({
		L"sfx_mob_geomancerHurt-001.ogg",
		L"sfx_mob_geomancerHurt-002.ogg" ,
		L"sfx_mob_geomancerHurt-003.ogg",
		L"sfx_mob_geomancerHurt-004.ogg" });

	m_pStat->SetMaxHP(100);

	CController* pController = Add_Component<CGeomancerController>(L"Proto_GeomancerController", L"Proto_GeomancerController", ID_DYNAMIC);
	pController->SetOwner(this);

	return S_OK;
}

void CGeomancer::AnimationEvent(const string& strEvent)
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
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.5f);
	}
}

_int CGeomancer::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

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


	if (m_bAttackFire)
	{
		size_t iSize = m_vecWallPos.size();

		if (iSize <= 4)//bomb
		{
			for (auto& wallPos : m_vecWallPos)
				CObjectFactory::Create<CGeomancerWall>("GeoWall_Boom", L"GeoWall_Boom", wallPos);
		}
		else // wall
		{
			for (auto& wallPos : m_vecWallPos)
				CObjectFactory::Create<CGeomancerWall>("GeoWall_Normal", L"GeoWall_Normal", wallPos);
		}
	
		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"sfx_mob_geomancerAttack-001.ogg",
			L"sfx_mob_geomancerAttack-002.ogg" }, 
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.3f);

		m_bAttackFire = false;
	}


	return OBJ_NOEVENT;
}


void CGeomancer::Free()
{
	CMonster::Free();
}

CGeomancer* CGeomancer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	CGeomancer* pInstance = new CGeomancer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CGeomancer::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			CSoundMgr::GetInstance()->PlaySoundRandom({
				L"sfx_mob_geomancerDeath-001.ogg",
				L"sfx_mob_geomancerDeath-002.ogg",
				L"sfx_mob_geomancerDeath-003.ogg" },
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
		if (m_pCurAnim != m_pIdleAnim)
			StopCurAnimation();
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
		RotateToTargetPos(m_vTargetPos, true);
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