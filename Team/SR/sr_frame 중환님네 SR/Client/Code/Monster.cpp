#include "stdafx.h"
#include "Monster.h"
#include "StatComponent.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CSkeletalCube(pGraphicDev)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CSkeletalCube(rhs)
{
}


CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_Object()
{
	CSkeletalCube::Ready_Object();

	m_pColl = Add_Component<CCollisionCom>(L"Proto_CollisionCom", L"Proto_CollisionCom", ID_DYNAMIC);
	m_pColl->SetOwner(this);
	m_pColl->SetOwnerTransform(m_pRootPart->pTrans);
	m_pColl->SetCollOffset(_vec3{0.f, 1.5f, 0.f});
	m_pColl->SetRadius(1.f);
	m_pColl->SetCollType(COLL_ENEMY);

	m_pStat = Add_Component<CStatComponent>(L"Proto_StatCom", L"Proto_StatCom", ID_DYNAMIC);
	m_pStat->SetMaxHP(100);
	m_pStat->SetTransform(m_pRootPart->pTrans);
	m_pStat->SetOwner(this);

	return S_OK;
}

_int CMonster::Update_Object(const _float& fTimeDelta)
{
	CSkeletalCube::Update_Object(fTimeDelta);
	DEBUG_SPHERE(m_pColl->GetCollPos(), m_pColl->GetRadius(), 0.1f);
	// 테스트 코드
	// _vec3 diff = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS] - m_pRootPart->pTrans->m_vInfo[INFO_POS];
	// D3DXVec3Normalize(&diff, &diff);
	// m_pRootPart->pTrans->m_vInfo[INFO_POS] += diff * fTimeDelta * 2.f;

	return OBJ_NOEVENT;
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	auto pInst = new CMonster(pGraphicDev);
	if (FAILED(pInst->Ready_Object()))
	{
		Safe_Release(pInst);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInst->LoadSkeletal(wstrPath);

	return pInst;
}

void CMonster::Free()
{
	CGameObject::Free();
}

void CMonster::RotateToTargetPos(const _vec3& vTargetPos, bool bReverse)
{
	_vec3 vLook = vTargetPos - m_pRootPart->pTrans->m_vInfo[INFO_POS];
	if (bReverse) vLook = -vLook;

	D3DXVec3Normalize(&vLook, &vLook);

	const _vec2 v2Look{0.f, 1.f};
	_vec2 v2ToDest{vLook.x, vLook.z};

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		m_pRootPart->pTrans->m_vAngle.y = -acosf(fDot);
	else
		m_pRootPart->pTrans->m_vAngle.y = acosf(fDot);
}
