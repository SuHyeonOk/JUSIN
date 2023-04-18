#include "stdafx.h"
#include "GeomancerController.h"
#include "Geomancer.h"
#include "Player.h"

CGeomancerController::CGeomancerController()
{
}

CGeomancerController::CGeomancerController(const CGeomancerController& rhs)
{
	m_fCurWallCoolTime = 4.f + CGameUtilMgr::GetRandomFloat(-1.f, 3.f);
	m_fCurBombCoolTime = CGameUtilMgr::GetRandomFloat(0.f, 3.f);
}

CGeomancerController::~CGeomancerController()
{
}

_int CGeomancerController::Update_Component(const _float& fTimeDelta)
{
	// cooltime
	if (m_fCurWallCoolTime < m_fWallCoolTime)
		m_fCurWallCoolTime += fTimeDelta;
	if (m_fCurBombCoolTime < m_fBombCoolTime)
		m_fCurBombCoolTime += fTimeDelta;

	if (m_iTick++ < 20) return 0;
	m_iTick = 0;

	CGeomancer* pGeomancer = dynamic_cast<CGeomancer*>(m_pOwner);
	NULL_CHECK_RETURN(pGeomancer, 0);

	_vec3 vPos = pGeomancer->Get_Component<Engine::CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
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
	if (m_fCurWallCoolTime >= m_fWallCoolTime)
	{
		m_fCurWallCoolTime = 0.f;

		_vec3 vToTarget = vTargetPos - vPos;
		D3DXVec3Normalize(&vToTarget, &vToTarget);
		_matrix matYaw;
		D3DXMatrixRotationY(&matYaw, D3DXToRadian(30.f));

		D3DXVec3TransformNormal(&vToTarget, &vToTarget, &matYaw);

		for (int i = 0; i < 8; ++i)
		{
			D3DXVec3TransformNormal(&vToTarget, &vToTarget, &matYaw);
			m_vecWallPos.push_back(vTargetPos + (vToTarget * 3.f));
		}


		pGeomancer->WallSpawn(m_vecWallPos);
		m_vecWallPos.clear();
		return 0;
	}

	if (m_fCurBombCoolTime > m_fBombCoolTime)
	{
		m_fCurBombCoolTime = 0.f;

		const int iBombCnt = rand() % 3 + 1;
		for (int i = 0; i < iBombCnt; ++i)
		{
			_vec3 vRand = {_float(rand() % 50 - 25) * 0.1f, 0.f,  _float(rand() % 50 - 25) * 0.1f};
			m_vecWallPos.push_back(vRand + vTargetPos);
		}
		
		pGeomancer->WallSpawn(m_vecWallPos);
		m_vecWallPos.clear();
		return 0;
	}

	if (fTargetDist < m_fRunDist)
	{
		pGeomancer->Run(vTargetPos);
		return 0;
	}
	


	return 0;
}

CComponent* CGeomancerController::Clone()
{
	return new CGeomancerController(*this);
}

void CGeomancerController::Free()
{
	CController::Free();
}

CGeomancerController* CGeomancerController::Create()
{
	return new CGeomancerController;
}
