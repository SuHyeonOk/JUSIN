#include "stdafx.h"
#include "..\Header\EndermanController.h"
#include "Enderman.h"

CEndermanController::CEndermanController()
{
}

CEndermanController::CEndermanController(const CEndermanController & rhs)
{
}


CEndermanController::~CEndermanController()
{
}

_int CEndermanController::Update_Component(const _float & fTimeDelta)
{
	m_fCurAttackCollTime += fTimeDelta;
	m_fCurClockingCollTime += fTimeDelta;
	m_fCurArmAttackCollTime += fTimeDelta;
	m_fCurPistolCollTime += fTimeDelta;

	CEnderman* pEnderman = dynamic_cast<CEnderman*>(m_pOwner);
	NULL_CHECK_RETURN(pEnderman, 0);

	_vec3 vPos = pEnderman->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	CPlayer* pTargetPlayer = nullptr;
	_float fTargetDist = 20.f;
	_vec3 vTargetPos;
	_vec3 vTargetLook;

	for (auto& ele : Get_Layer(LAYER_PLAYER)->Get_MapObject())
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(ele.second))
		{
			vTargetPos = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
			vTargetLook = pPlayer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_LOOK];
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

	pEnderman->SetTarget(vTargetPos);

	
	if (m_fCurClockingCollTime > m_fClockingCollTime)
	{
		pEnderman->DoClocking(vTargetLook);
		m_fCurClockingCollTime = 0.f;
		return 0;
	}

	
	if (m_fCurArmAttackCollTime > m_fArmAttackCollTime)
	{
		pEnderman->DoArmAttack();
		m_fCurArmAttackCollTime = 0.f;
		return 0;
	}

	if (m_fCurPistolCollTime > m_fPistolCollTime)
	{
		pEnderman->DoPistol();
		m_fCurPistolCollTime = 0.f;
		return 0;
	}

	if (fTargetDist < 4.f && m_fCurAttackCollTime > m_fAttakCollTime)
	{
		_int irandom = rand() % 2;
		if (irandom == 0)
		{
			pEnderman->DoChop();
			m_fCurAttackCollTime = 0.f;
		}
		else
		{
			pEnderman->DoSmash();
			m_fCurAttackCollTime = 0.f;
		}
	}
	else if (fTargetDist >= 2.f)
	{
		pEnderman->WalkToTarget();
	}
	else
	{
		pEnderman->DoIdle();
	}



	return 0;
}

CComponent * CEndermanController::Clone()
{
	return new CEndermanController(*this);
}

void CEndermanController::Free()
{
	CController::Free();
}

CEndermanController * CEndermanController::Create()
{
	return new CEndermanController;
}
