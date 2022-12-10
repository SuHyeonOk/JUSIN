#include "stdafx.h"
#include "..\public\M_Monster.h"

#include "GameInstance.h"
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
}

void CM_Monster::Late_Tick(const _double& TimeDelta)
{
	__super::Late_Tick(TimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CM_Monster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

_bool CM_Monster::RandomMove(CTransform* pTransform, _float4 f4FirstPos, _float fRange, _double TimeDelta, _float fStart, _float fEnd)
{
	if (!m_bRandomPos)
	{
		// ·£´ýÇÑ ÁÂÇ¥¸¦ ±¸ÇÑ´Ù.
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

		if (fRange < fCheckDistance)
			m_bRandomPos = false;
		else
			m_bRandomPos = true;
	}

	// ·£´ýÇÑ ÁÂÇ¥¸¦ ¹ÞÀº °÷ À¸·Î ÀÌµ¿ÇÑ´Ù.
	_vector vRandomPos;
	vRandomPos = XMLoadFloat4(&m_f4RandomPos);

	pTransform->LookAt(vRandomPos);
	pTransform->Chase(vRandomPos, TimeDelta);

	// ·£´ý ÁÂÇ¥¿Í ÇöÀç ÁÂÇ¥°¡ °¡±î¿öÁö¸é...
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
