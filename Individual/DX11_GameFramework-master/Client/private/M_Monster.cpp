#include "stdafx.h"
#include "..\public\M_Monster.h"

#include "GameInstance.h"
#include "Obj_Manager.h"
#include "Utilities_Manager.h"

CM_Monster::CM_Monster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CM_Monster::CM_Monster(const CM_Monster & rhs)
	: CGameObject(rhs)
{

}

HRESULT CM_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Monster::Initialize(void * pArg)
{
	MONSTERDESC		MonsterDesc;
	ZeroMemory(&MonsterDesc, sizeof(MonsterDesc));

	if (nullptr != pArg)
		memcpy(&MonsterDesc, pArg, sizeof(MONSTERDESC));

	if (FAILED(__super::Initialize(&MonsterDesc)))
		return E_FAIL;
	
	m_pTransformCom->Set_Pos();

	return S_OK;
}

void CM_Monster::Tick(const _double& TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pColliderCom[COLLTYPE_AABB]->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_bPlayer_Attack)
	{
		m_dPlayer_Attack_TimeAcc += TimeDelta;
		if (0.7 < m_dPlayer_Attack_TimeAcc)
		{
			m_tMonsterInfo.iHp -= CObj_Manager::GetInstance()->Get_Player_Attack();
			m_tMonsterInfo.eState = m_tMonsterInfo.HIT;

			m_bPlayer_Attack = false;
			m_dPlayer_Attack_TimeAcc = 0;
		}
	}
}

void CM_Monster::Late_Tick(const _double& TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance::GetInstance()->Add_ColGroup(CCollider_Manager::COL_MONSTER, this);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Monster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CObj_Manager::GetInstance()->Get_NavigationRender())
	{
		if (nullptr != m_pColliderCom[COLLTYPE_AABB])
			m_pColliderCom[COLLTYPE_AABB]->Render();
	}

	return S_OK;
}

void CM_Monster::On_Collision(CGameObject * pOther)
{
	if (CObj_Manager::PLAYERINFO::STATE::ATTACK == CObj_Manager::GetInstance()->Get_Current_Player().eState)
	{
		if(L"Finn_Weapon" == pOther->Get_Tag() || L"Jake_Weapon" == pOther->Get_Tag())
			m_bPlayer_Attack = true;
	}
}

_bool CM_Monster::Random_Move(CTransform * pTransform, _float4 f4CenterPos, _double TimeDelta, _double dMoveTime, _float fRange)
{
	// 이동 시킬 시간을 담는다.
	m_bRandomMove_TimeAcc += TimeDelta;
	if (dMoveTime < m_bRandomMove_TimeAcc)
	{
		m_bRandomMove_TimeAcc = 0;
		return false;
	}

	_vector	vMyPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vCenterPos = XMLoadFloat4(&f4CenterPos);
	_vector vDistance = vCenterPos - vMyPos;
	_float	fDiatance = XMVectorGetX(XMVector3Length(vDistance));

	pTransform->Go_Straight(TimeDelta);

	if (fRange < fDiatance)	// 일정 범위를 나가면
	{
		pTransform->Chase(vCenterPos, TimeDelta);	// 원점으로 돌아가고
		_float fRandomAxis = CUtilities_Manager::GetInstance()->Get_Random(0.f, 360.f);	// 랜덤으로
		pTransform->Rotation(pTransform->Get_State(CTransform::STATE_UP), fRandomAxis);	// Look 을 변경한다.
	}

	return true;
}

_bool CM_Monster::RandomMove(CTransform* pTransform, _float4 f4FirstPos, _float fRange, _double TimeDelta, _float fStart, _float fEnd)
{
	if (!m_bRandomPos)
	{
		// 랜덤한 좌표를 구한다.
		_float fRandom_1 = CUtilities_Manager::GetInstance()->Get_Random(fStart, fEnd);
		_float fRandom_2 = CUtilities_Manager::GetInstance()->Get_Random(fStart, fEnd);

		_vector vMyPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
		_float4 f4MyPos;
		XMStoreFloat4(&f4MyPos, vMyPos);

		_int iRandom = CUtilities_Manager::GetInstance()->Get_Random(0, 7);
		if (0 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x + fRandom_1), f4MyPos.y, (f4MyPos.z + fRandom_2), 1.f);
		else if (1 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z - fRandom_1), 1.f);
		else if (2 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x + fRandom_1), f4MyPos.y, (f4MyPos.z - fRandom_2), 1.f);
		else if (3 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z + fRandom_1), 1.f);
		else if (4 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z - fRandom_1), 1.f);
		else if (5 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x + fRandom_1), f4MyPos.y, (f4MyPos.z - fRandom_2), 1.f);
		else if (6 == iRandom)
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_2), f4MyPos.y, (f4MyPos.z + fRandom_1), 1.f);
		else
			m_f4RandomPos = _float4((f4MyPos.x - fRandom_1), f4MyPos.y, (f4MyPos.z + fRandom_2), 1.f);

		_vector vRandomPos, vFirstPos;
		vRandomPos = XMLoadFloat4(&m_f4RandomPos);
		vFirstPos = XMLoadFloat4(&f4FirstPos);
		_vector vCheckDistance = vRandomPos - vFirstPos;
		_float fCheckDistance = XMVectorGetX(XMVector3Length(vCheckDistance));

		m_bRandomPos = true;
	}

	// 랜덤한 좌표를 받은 곳 으로 이동한다.
	_vector vRandomPos;
	vRandomPos = XMLoadFloat4(&m_f4RandomPos);

	pTransform->LookAt(vRandomPos);
	pTransform->Chase(vRandomPos, TimeDelta);

	// 랜덤 좌표와 현재 좌표가 가까워지면...
	_vector vMyPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDistance = vRandomPos - vMyPos;
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	if (0.1f > fDistance)
	{
		m_bRandomPos = false;
		return true;
	}
	else
		return false;
}

void CM_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom[COLLTYPE_AABB]);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
