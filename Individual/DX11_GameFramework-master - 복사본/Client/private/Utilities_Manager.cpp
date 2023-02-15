#include "stdafx.h"
#include "..\public\Utilities_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUtilities_Manager)

CUtilities_Manager::CUtilities_Manager()
{
	// ���� �Լ� ��� ���
	//_int iRan = CUtilities_Manager::GetInstance()->Get_Random(0, 99);
	//_float fRan = CUtilities_Manager::GetInstance()->Get_Random(0.f, 99.f);

	//cout << "====================" << endl;
	//cout << iRan << endl;
	//cout << fRan << endl;
	//cout << "====================" << endl;
}

_int CUtilities_Manager::Get_Random(_int iStart, _int iEnd)
{
	// �õ尪�� ��� ���� random_device ����.
	std::random_device rd;

	// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
	std::mt19937 gen(rd());

	// 0 ���� 99 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
	std::uniform_int_distribution<int> dis(iStart, iEnd);

	return	dis(gen);
}

_float CUtilities_Manager::Get_Random(_float fStart, _float fEnd)
{
	// �õ尪�� ��� ���� random_device ����.
	std::random_device rd;

	// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
	std::mt19937 gen(rd());

	// 0 ���� 99 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
	std::uniform_real_distribution <float> dis(fStart, fEnd);

	return	dis(gen);
}

_bool CUtilities_Manager::Get_RandomPos(CTransform* pTransform, _float4 f4FirstPos,  _float fRange, _double TimeDelta, _float fStart, _float fEnd)
{
	if (!m_bRandomPos)
	{
		// ������ ��ǥ�� ���Ѵ�.
		_float fRandom_1 = Get_Random(fStart, fEnd);
		_float fRandom_2 = Get_Random(fStart, fEnd);

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

	// ������ ��ǥ�� ���� �� ���� �̵��Ѵ�.
	_vector vRandomPos;
	vRandomPos = XMLoadFloat4(&m_f4RandomPos);

	pTransform->LookAt(vRandomPos);
	pTransform->Chase(vRandomPos, TimeDelta);

	// ���� ��ǥ�� ���� ��ǥ�� ���������...
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

	//if (!m_bRandomPos)
	//{
	//	_float fRandom = CUtilities_Manager::GetInstance()->Get_Random(0.5f, 3.f);

	//	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	_float4 f4MyPos;
	//	XMStoreFloat4(&f4MyPos, vMyPos);

	//	_int iRandom = CUtilities_Manager::GetInstance()->Get_Random(0, 3);
	//	if (0 == iRandom)
	//		m_f4RandomPos = _float4((f4MyPos.x + fRandom), f4MyPos.y, (f4MyPos.z + fRandom), 1.f);
	//	else if (1 == iRandom)
	//		m_f4RandomPos = _float4((f4MyPos.x - fRandom), f4MyPos.y, (f4MyPos.z - fRandom), 1.f);
	//	else if (2 == iRandom)
	//		m_f4RandomPos = _float4((f4MyPos.x + fRandom), f4MyPos.y, (f4MyPos.z - fRandom), 1.f);
	//	else if (3 == iRandom)
	//		m_f4RandomPos = _float4((f4MyPos.x - fRandom), f4MyPos.y, (f4MyPos.z + fRandom), 1.f);

	//	_vector vRandomPos, vFirstPos;
	//	vRandomPos = XMLoadFloat4(&m_f4RandomPos);
	//	vFirstPos = XMLoadFloat4(&m_f4First_Pos);
	//	_vector vCheckDistance = vRandomPos - vFirstPos;
	//	_float fCheckDistance = XMVectorGetX(XMVector3Length(vCheckDistance));

	//	if (3.f < fCheckDistance)
	//		m_bRandomPos = false;
	//	else
	//		m_bRandomPos = true;
	//}

	//// ������ ��ǥ�� ���� �� ���� �̵��Ѵ�.
	//_vector vRandomPos;
	//vRandomPos = XMLoadFloat4(&m_f4RandomPos);

	//m_pTransformCom->LookAt(vRandomPos);
	//m_pTransformCom->Chase(vRandomPos, TimeDelta);

	//// ���� ��ǥ�� ���� ��ǥ�� ���������...
	//_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_vector vDistance = vRandomPos - vMyPos;
	//_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	//if (0.1f > fDistance)
	//	m_bRandomPos = false;
}

void CUtilities_Manager::Free()
{
}