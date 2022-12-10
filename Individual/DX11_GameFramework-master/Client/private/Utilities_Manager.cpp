#include "stdafx.h"
#include "..\public\Utilities_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUtilities_Manager)

CUtilities_Manager::CUtilities_Manager()
{
	// 랜덤 함수 사용 방법
	//_int iRan = CUtilities_Manager::GetInstance()->Get_Random(0, 99);
	//_float fRan = CUtilities_Manager::GetInstance()->Get_Random(0.f, 99.f);

	//cout << "====================" << endl;
	//cout << iRan << endl;
	//cout << fRan << endl;
	//cout << "====================" << endl;
}

_int CUtilities_Manager::Get_Random(_int iStart, _int iEnd)
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;

	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());

	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_int_distribution<int> dis(iStart, iEnd);

	return	dis(gen);
}

_float CUtilities_Manager::Get_Random(_float fStart, _float fEnd)
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;

	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());

	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_real_distribution <float> dis(fStart, fEnd);

	return	dis(gen);
}

_bool CUtilities_Manager::Get_RandomPos(CTransform* pTransform, _float4 f4FirstPos,  _float fRange, _double TimeDelta, _float fStart, _float fEnd)
{
	if (!m_bRandomPos)
	{
		// 랜덤한 좌표를 구한다.
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

	//// 랜덤한 좌표를 받은 곳 으로 이동한다.
	//_vector vRandomPos;
	//vRandomPos = XMLoadFloat4(&m_f4RandomPos);

	//m_pTransformCom->LookAt(vRandomPos);
	//m_pTransformCom->Chase(vRandomPos, TimeDelta);

	//// 랜덤 좌표와 현재 좌표가 가까워지면...
	//_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_vector vDistance = vRandomPos - vMyPos;
	//_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

	//if (0.1f > fDistance)
	//	m_bRandomPos = false;
}

void CUtilities_Manager::Free()
{
}