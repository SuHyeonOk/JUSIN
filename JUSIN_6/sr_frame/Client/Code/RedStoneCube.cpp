#include "stdafx.h"
#include "..\Header\RedStoneCube.h"
#include "StatComponent.h"
#include "RedStoneCubeController.h"

CRedStoneCube::CRedStoneCube(LPDIRECT3DDEVICE9 pGraphicDev) : CMonster(pGraphicDev)
{
}

CRedStoneCube::CRedStoneCube(const CMonster& rhs) : CMonster(rhs)
{
}

CRedStoneCube::~CRedStoneCube()
{
}

HRESULT CRedStoneCube::Ready_Object()
{
	CMonster::Ready_Object();

	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneCube/walk.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneCube/dead.anim");
	m_arrAnim[ANIM_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/RedStoneCube/attack.anim");
	m_pIdleAnim = &m_arrAnim[ANIM_WALK];
	m_pCurAnim = m_pIdleAnim;
	m_eState = WALK;
	m_fSpeed = 2.f;

	m_pStat->SetMaxHP(100);

	CController* pController = Add_Component<CRedStoneCubeController>(L"Proto_RedStoneCubeController", L"Proto_RedStoneCubeController", ID_DYNAMIC);
	pController->SetOwner(this);

	return S_OK;
}

void CRedStoneCube::AnimationEvent(const string& strEvent)
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
			L"sfx_mob_redstoneCubeWalk-001.ogg",
			L"sfx_mob_redstoneCubeWalk-002.ogg",
			L"sfx_mob_redstoneCubeWalk-003.ogg",
			L"sfx_mob_redstoneCubeWalk-004.ogg"},
			m_pRootPart->pTrans->m_vInfo[INFO_POS], 0.3f);
	}
}

_int CRedStoneCube::Update_Object(const _float& fTimeDelta)
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
	case WALK:
		m_pRootPart->pTrans->m_vInfo[INFO_POS] += m_pRootPart->pTrans->m_vInfo[INFO_LOOK] * m_fSpeed * fTimeDelta;
		break;
	case ATTACK:
		break;
	case STUN:
		break;
	case DEAD:
		if (m_pCurAnim)
		break;
	default:
		break;
	}


	return OBJ_NOEVENT;
}

void CRedStoneCube::LateUpdate_Object()
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
				->TakeDamage(15, m_pRootPart->pTrans->m_vInfo[INFO_POS], this);
		}
		DEBUG_SPHERE(vAttackPos, 1.f, 1.f);
		IM_LOG("Fire");

		m_bAttackFire = false;
	}
}

void CRedStoneCube::Free()
{
	CMonster::Free();
}

CRedStoneCube* CRedStoneCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	CRedStoneCube* pInstance = new CRedStoneCube(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CRedStoneCube::StateChange()
{
	if (m_pStat->IsDead())
	{
		if (m_bReserveStop == false)
		{
			m_eState = DEAD;
			PlayAnimationOnce(&m_arrAnim[ANIM_DEAD], true);
			m_bAttack = false;
			m_bCanPlayAnim = false;
			m_pColl->SetStop();
		}
		return;
	}

	if (m_pStat->IsStun())
	{
		m_eState = STUN;
		m_bAttack = false;
		StopCurAnimation();
		return;
	}

	if (m_bAttack && m_bCanPlayAnim)
	{
		m_eState = ATTACK;
		RotateToTargetPos(m_vTargetPos);
		PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK]);
		m_bCanPlayAnim = false;
		m_bAttack = false;
		return;
	}

	if (m_bCanPlayAnim)
	{
		m_eState = WALK;
		RotateToTargetPos(m_vTargetPos);
		m_pIdleAnim = &m_arrAnim[ANIM_WALK];
		m_pCurAnim = &m_arrAnim[ANIM_WALK];
		return;
	}

}