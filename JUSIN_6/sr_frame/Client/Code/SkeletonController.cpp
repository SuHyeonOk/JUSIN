#include "stdafx.h"
#include "SkeletonController.h"
#include "Skeleton.h"
#include "Player.h"

CSkeletonController::CSkeletonController()
{
}

CSkeletonController::CSkeletonController(const CSkeletonController& rhs)
{
}

CSkeletonController::~CSkeletonController()
{
}

_int CSkeletonController::Update_Component(const _float& fTimeDelta)
{
	// cooltime
	if (m_fCurAttackCoolTime < m_fAttackCoolTime)
		m_fCurAttackCoolTime += fTimeDelta;

	if (m_iTick++ < 20) return 0;
	m_iTick = 0;

	CSkeleton* pSkeleton = dynamic_cast<CSkeleton*>(m_pOwner);
	NULL_CHECK_RETURN(pSkeleton, 0);

	_vec3 vPos = pSkeleton->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	CPlayer* pTargetPlayer = nullptr;
	_vec3 vTargetPos;
	_float fTargetDist = 9999.f;

	for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
		{
			vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDiff = vPos - vTargetPos;
			_float fDist = D3DXVec3Length(&vDiff);

			if (fDist < m_fAttackRange && fTargetDist > fDist) // 화면 밖에서 공격할 수 있는 범위인듯
			{
				pTargetPlayer = pPlayer;
				fTargetDist = fDist;
			}
		}
	}

	if (pTargetPlayer == nullptr) return 0;


	// 공격 가능
	if (m_fCurAttackCoolTime >= m_fAttackCoolTime)
	{
		m_fCurAttackCoolTime = 0.f;
		pSkeleton->AttackPress(vTargetPos);
		return 0;
	}

	if (fTargetDist < m_fRunDist)
	{
		pSkeleton->Run(vTargetPos);
		return 0;
	}



	return 0;
}

CComponent* CSkeletonController::Clone()
{
	return new CSkeletonController(*this);
}

void CSkeletonController::Free()
{
	CController::Free();
}

CSkeletonController* CSkeletonController::Create()
{
	return new CSkeletonController;
}
