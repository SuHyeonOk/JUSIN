#include "stdafx.h"
#include "..\Header\Skeleton.h"
#include "StatComponent.h"
#include "SkeletonController.h"
#include "AbstFactory.h"

CSkeleton::CSkeleton(LPDIRECT3DDEVICE9 pGraphicDev) : CMonster(pGraphicDev)
{
}

CSkeleton::CSkeleton(const CMonster& rhs) : CMonster(rhs)
{
}

CSkeleton::~CSkeleton()
{
}

HRESULT CSkeleton::Ready_Object(const wstring& wstrPath)
{
	CMonster::Ready_Object();

	if (!wstrPath.empty())
		LoadSkeletal(wstrPath);

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Skeleton/idle.anim");
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Skeleton/walk.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Skeleton/dead.anim");
	m_arrAnim[ANIM_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/Skeleton/attack.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_IDLE];
	m_pCurAnim = m_pIdleAnim;
	m_eState = IDLE;
	m_fSpeed = 3.f;

	m_pStat->SetMaxHP(100);

	m_pStat->SetHurtSound({
		L"sfx_mob_skeletonHurt-001.ogg",
		L"sfx_mob_skeletonHurt-002.ogg" ,
		L"sfx_mob_skeletonHurt-003.ogg",
		L"sfx_mob_skeletonHurt-004.ogg" });

	CController* pController = Add_Component<CSkeletonController>(L"Proto_SkeletonController", L"Proto_SkeletonController", ID_DYNAMIC);
	pController->SetOwner(this);

	auto pPart = m_mapParts.find("bow")->second;
	pPart->pBuf = Add_Component<CVoxelTex>(L"Proto_VoxelTex_Bow", L"Proto_VoxelTex_Bow_Bow", ID_STATIC);
	pPart->iTexIdx = 4;

	return S_OK;
}

void CSkeleton::AnimationEvent(const string& strEvent)
{
	if (strEvent == "AttackFire")
	{
		//m_bAttackFire = true;
		
		CBulletFactory::Create<CGameObject>("EnemyNormalArrow", L"SkeletonArrow", 
		{10.f, false, COLL_ENEMY_BULLET, ARROW_NORMAL}, 
			m_pRootPart->pTrans->m_vInfo[INFO_POS] + _vec3{0.f, 1.f, 0.f}, 
			m_vTargetPos);
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
			L"sfx_mob_skeletonStep-001.ogg",
			L"sfx_mob_skeletonStep-002.ogg",
			L"sfx_mob_skeletonStep-003.ogg",
			L"sfx_mob_skeletonStep-004.ogg"},
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.3f);
	}
}

_int CSkeleton::Update_Object(const _float& fTimeDelta)
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


	return OBJ_NOEVENT;
}

void CSkeleton::LateUpdate_Object()
{
	CMonster::LateUpdate_Object();
}

void CSkeleton::Free()
{
	CMonster::Free();
}

CSkeleton* CSkeleton::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	CSkeleton* pInstance = new CSkeleton(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(wstrPath)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSkeleton::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			CSoundMgr::GetInstance()->PlaySoundRandom({
				L"sfx_mob_skeletonDeath-001.ogg",
				L"sfx_mob_skeletonDeath-002.ogg",
				L"sfx_mob_skeletonDeath-003.ogg",
				L"sfx_mob_skeletonDeath-004.ogg" },
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