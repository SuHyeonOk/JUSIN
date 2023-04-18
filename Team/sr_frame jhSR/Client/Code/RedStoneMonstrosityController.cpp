#include "stdafx.h"
#include "RedStoneMonstrosityController.h"
#include "RedStoneMonstrosity.h"
#include "Player.h"

CRedStoneMonstrosityController::CRedStoneMonstrosityController()
{
	//m_pTargetPlayer->AddRef();
}

CRedStoneMonstrosityController::CRedStoneMonstrosityController(const CRedStoneMonstrosityController& rhs)
{
	//m_pTargetPlayer->AddRef();
}

CRedStoneMonstrosityController::~CRedStoneMonstrosityController()
{
}

_int CRedStoneMonstrosityController::Update_Component(const _float& fTimeDelta)
{
	// cooltime
	{	
		m_fCurChopCoolTime += fTimeDelta;
		m_fCurSpitCoolTime += fTimeDelta;
		m_fCurSummonCoolTime += fTimeDelta;
		m_fCurWindmillCoolTime += fTimeDelta;
	}

	CRedStoneMonstrosity* pRedStoneMonstrosity = dynamic_cast<CRedStoneMonstrosity*>(m_pOwner);
	NULL_CHECK_RETURN(pRedStoneMonstrosity, 0);

	_vec3 vPos = pRedStoneMonstrosity->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	_vec3 vTargetPos;
	_float fTargetDist = 15.f;


	
	for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
		{
			vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDiff = vTargetPos - vPos;
			_float fDist = D3DXVec3Length(&vDiff);

			if (fTargetDist > fDist) // 플레이어 감지
			{
				m_pTargetPlayer = pPlayer;
				fTargetDist = fDist;
			}
		}
	}

	if (m_pTargetPlayer == nullptr)
		return 0;
	

	if (m_fCurChopCoolTime >= m_fChopCoolTime && fTargetDist <= m_fChopDist)
	{
		m_fCurChopCoolTime = 0.f;
		pRedStoneMonstrosity->Chop(vTargetPos);
		return 0;
	}

	if (m_fCurSpitCoolTime >= m_fSpitCoolTime && fTargetDist <= m_fSpitDist)
	{
		m_fCurSpitCoolTime = 0.f;
		pRedStoneMonstrosity->Spit(vTargetPos);
		return 0;
	}

	if (m_fCurSummonCoolTime >= m_fSummonCoolTime)
	{
		m_fCurSummonCoolTime = 0.f;
		pRedStoneMonstrosity->Summon(vTargetPos);
		return 0;
	}

	if (m_fCurWindmillCoolTime >= m_fWindmillCoolTime && fTargetDist <= m_fWindmillDist)
	{
		m_fCurWindmillCoolTime = 0.f;
		pRedStoneMonstrosity->Windmill(vTargetPos);
		return 0;
	}

	pRedStoneMonstrosity->WalkToTarget(vTargetPos);

	return 0;
}

CComponent* CRedStoneMonstrosityController::Clone()
{
	return new CRedStoneMonstrosityController(*this);
}

void CRedStoneMonstrosityController::Free()
{
	//Safe_Release(m_pTargetPlayer);
	CController::Free();

}

CRedStoneMonstrosityController* CRedStoneMonstrosityController::Create()
{
	return new CRedStoneMonstrosityController;
}
