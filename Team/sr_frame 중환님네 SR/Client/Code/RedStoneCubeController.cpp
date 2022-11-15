#include "stdafx.h"
#include "RedStoneCubeController.h"
#include "RedStoneCube.h"
#include "Player.h"

CRedStoneCubeController::CRedStoneCubeController()
{
}

CRedStoneCubeController::CRedStoneCubeController(const CRedStoneCubeController& rhs)
{
}

CRedStoneCubeController::~CRedStoneCubeController()
{
}

_int CRedStoneCubeController::Update_Component(const _float& fTimeDelta)
{
	if (m_iTick++ < 20) return 0;
	m_iTick = 0;

	CRedStoneCube* pRedStoneCube = dynamic_cast<CRedStoneCube*>(m_pOwner);
	NULL_CHECK_RETURN(pRedStoneCube, 0);

	_vec3 vPos = pRedStoneCube->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	CPlayer* pTargetPlayer = nullptr;
	_vec3 vTargetPos;
	_float fTargetDist = 9999.f;

	for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
		{
			vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDiff = vTargetPos - vPos;
			_float fDist = D3DXVec3Length(&vDiff);

			if (fDist < m_fDetectRange && fTargetDist > fDist) // 플레이어 감지
			{
				pTargetPlayer = pPlayer;
				fTargetDist = fDist;
			}
		}
	}

	if (pTargetPlayer == nullptr) return 0;

	if (fTargetDist <= m_fAttackDist)
	{
		pRedStoneCube->AttackPress(vTargetPos);
		return 0;
	}

	if (fTargetDist > m_fAttackDist) // 공격 사거리 밖이라면 더 가까이 감
		pRedStoneCube->WalkToTarget(vTargetPos);



	return 0;
}

CComponent* CRedStoneCubeController::Clone()
{
	return new CRedStoneCubeController(*this);
}

void CRedStoneCubeController::Free()
{
	CController::Free();
}

CRedStoneCubeController* CRedStoneCubeController::Create()
{
	return new CRedStoneCubeController;
}
