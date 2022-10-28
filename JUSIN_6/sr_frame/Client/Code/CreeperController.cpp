#include "stdafx.h"
#include "CreeperController.h"
#include "Creeper.h"
#include "Player.h"

CCreeperController::CCreeperController()
{
}

CCreeperController::CCreeperController(const CCreeperController& rhs)
{
}

CCreeperController::~CCreeperController()
{
}

_int CCreeperController::Update_Component(const _float& fTimeDelta)
{
	if (m_bWalk) m_fExplosionCount += fTimeDelta;

	if (m_iTick++ < 20) return 0;
	m_iTick = 0;

	CCreeper* pCreeper = dynamic_cast<CCreeper*>(m_pOwner);
	NULL_CHECK_RETURN(pCreeper, 0);

	_vec3 vPos = pCreeper->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	_vec3 vTargetPos;
	

	for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
		{
			vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDiff = vTargetPos - vPos;
			_float fDist = D3DXVec3Length(&vDiff);
		
			if (fDist < m_fCountStartDist)		
				m_bWalk = true;
						
		}
	}

	if (m_bWalk)
	{
		pCreeper->AttackCountStart();
		if (m_fExplosionCount >= 2.5f)
		{
			pCreeper->Explosion(vTargetPos);
			return 0;
		}

		IM_LOG("%f" , m_fExplosionCount);
		pCreeper->WalkToTarget(vTargetPos);
	}
		
	return 0;
}

CComponent* CCreeperController::Clone()
{
	return new CCreeperController(*this);
}

void CCreeperController::Free()
{
	CController::Free();
}

CCreeperController* CCreeperController::Create()
{
	return new CCreeperController;
}
