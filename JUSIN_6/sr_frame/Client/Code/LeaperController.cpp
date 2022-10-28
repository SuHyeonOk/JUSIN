#include "stdafx.h"
#include "..\Header\LeaperController.h"
#include "Leaper.h"

CLeaperController::CLeaperController()
{
}

CLeaperController::CLeaperController(const CLeaperController & rhs)
{
}


CLeaperController::~CLeaperController()
{
}

_int CLeaperController::Update_Component(const _float & fTimeDelta)
{
	m_fCurAttackCollTime += fTimeDelta;
	m_fCurJumpCollTime += fTimeDelta;

	CLeaper* pLeaper = dynamic_cast<CLeaper*>(m_pOwner);
	NULL_CHECK_RETURN(pLeaper, 0);

	_vec3 vPos = pLeaper->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	CPlayer* pTargetPlayer = nullptr;
	_float fTargetDist = 20.f;
	_vec3 vTargetPos;
	

	for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
		{
			vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			_vec3 vDiff = vTargetPos - vPos;
			_float fDist = D3DXVec3Length(&vDiff);

			if (fTargetDist > fDist)
			{
				pTargetPlayer = pPlayer;
				fTargetDist = fDist;
			}
		}
	}

	if (pTargetPlayer == nullptr)
		return 0;

	pLeaper->SetTarget(vTargetPos);


	if (fTargetDist < 3.f && m_fCurAttackCollTime > m_fAttakCollTime)
	{	
		pLeaper->DoAttack();
		m_fCurAttackCollTime = 0.f;
		return 0;

	}

	if (fTargetDist < 10.f && m_fCurJumpCollTime > m_fJumpCollTime)
	{
		pLeaper->DoJump();
		m_fCurJumpCollTime = 0.f;
		return 0;
	}

	fTargetDist >= 2.f ? pLeaper->WalkToTarget() : pLeaper->DoIdle();

	return 0;
}

CComponent * CLeaperController::Clone()
{
	return new CLeaperController(*this);
}

void CLeaperController::Free()
{
	CController::Free();
}

CLeaperController * CLeaperController::Create()
{
	return new CLeaperController;
}
