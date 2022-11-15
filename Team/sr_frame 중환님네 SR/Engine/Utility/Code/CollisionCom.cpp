#include "..\..\Header\CollisionCom.h"
#include "Collider.h"
#include "Transform.h"


CCollisionCom::CCollisionCom()
{
}

CCollisionCom::CCollisionCom(const CCollisionCom& rhs): CComponent(rhs)
{
}

CCollisionCom::~CCollisionCom()
{
}

_int CCollisionCom::Update_Component(const _float& fTimeDelta)
{
	if (m_bStop == false)
		CCollider::GetInstance()->Add_CollisionCom(this);
	return 0;
}

void CCollisionCom::LateUpdate_Component()
{
	CComponent::LateUpdate_Component();
}

CComponent* CCollisionCom::Clone()
{
	return new CCollisionCom(*this);
}

void CCollisionCom::Free()
{
	CComponent::Free();
}

CCollisionCom* CCollisionCom::Create()
{
	return new CCollisionCom();
}

void CCollisionCom::CollisionDynamic(CCollisionCom* pOther, BLOCKING_TYPE eType)
{
	if (eType == PUSH_EACH_OTHER)
	{
		_vec3& vPos = m_pOwnerTrans->m_vInfo[INFO_POS];
		_vec3 vCollPos = GetCollPos();

		_vec3& vPosOther = pOther->GetTransform()->m_vInfo[INFO_POS];
		_vec3 vCollPosOther = pOther->GetCollPos();

		_vec3 vToMe = vCollPos - vCollPosOther;
		_float fMoved = (m_fRadius + pOther->GetRadius() - D3DXVec3Length(&vToMe)) * 0.5f;

		D3DXVec3Normalize(&vToMe, &vToMe);

		vPos += fMoved * vToMe;
		vPosOther += -fMoved * vToMe;
	}
	else if (eType == PUSH_OTHER_ONLY)
	{
		_vec3 vCollPos = GetCollPos();

		_vec3& vPosOther = pOther->GetTransform()->m_vInfo[INFO_POS];
		_vec3 vCollPosOther = pOther->GetCollPos();

		_vec3 vToMe = vCollPos - vCollPosOther;
		_float fMoved = (m_fRadius + pOther->GetRadius() - D3DXVec3Length(&vToMe));

		D3DXVec3Normalize(&vToMe, &vToMe);

		vPosOther += -fMoved * vToMe;
	}
	else if (eType == CUSTOM_EVENT)
	{
		if (m_pCollisionDynamic) m_pCollisionDynamic(pOther);
	}
}

void CCollisionCom::CollisionStatic(const _vec3& vCenter, _float fRadius)
{
	if ((m_eType == COLL_ENEMY_BULLET || m_eType == COLL_PLAYER_BULLET) && m_pCollisionStatic)
		m_pCollisionStatic(vCenter, fRadius);
	else
	{
		_vec3& vPos = m_pOwnerTrans->m_vInfo[INFO_POS];
		_vec3 vCollPos = GetCollPos();
		
		_vec3 vDiff = vCollPos - vCenter;
		const _vec2 vXZDiff{vDiff.x, vDiff.z};
		_float fMoved = m_fRadius + fRadius - D3DXVec2Length(&vXZDiff);
		
		D3DXVec3Normalize(&vDiff, &vDiff);
		
		vPos += fMoved * vDiff;

		if (m_pStaticCallback)
			m_pStaticCallback();
	}
}
