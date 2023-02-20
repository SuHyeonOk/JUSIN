#include "stdafx.h"
#include "..\public\Camera_Dynamic.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eCameraInfo, pArg, sizeof(CAMERAINFO));

	ZeroMemory(&m_CameraDesc, sizeof m_CameraDesc);

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(m_CameraDesc));

	m_CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	m_CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	m_CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	m_CameraDesc.TransformDesc.fSpeedPerSec = 2.f;
	m_CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	m_CameraDesc.TransformDesc.f3Pos = m_eCameraInfo.f3Pos;

	if (FAILED(CCamera::Initialize(&m_CameraDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fYZ_Move);

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_V))
	//{
	//	//////////////////////////// 디버그용
	//	_vector vddMyPos;
	//	vddMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	_float4	f4ddMyPos;
	//	XMStoreFloat4(&f4ddMyPos, vddMyPos);

	//	cout << "카메라" << f4ddMyPos.x << " | " << f4ddMyPos.y << " | " << f4ddMyPos.z << endl;
	//	//////////////////////////// 디버그용
	//}

	//RELEASE_INSTANCE(CGameInstance);

	CurrentCamera_Tick(TimeDelta);

	__super::Tick(TimeDelta);
}

void CCamera_Dynamic::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);	

}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::SetUp_Components()
{

	return S_OK;
}

void CCamera_Dynamic::Shake_Camera(_double TimeDelta)
{
	if (!m_bShake)
		return;

	m_dShakeTimeNow += TimeDelta;

	_float fRand = 0.f;

	if (m_dShakeTime > m_dShakeTimeNow)
	{
		fRand = (rand() % (m_iShakePower * 2) - m_iShakePower * 0.5f) * 0.03f;
		m_CameraDesc.vAt.y += fRand;
	}
	else
	{
		m_dShakeTimeNow = 0;
		m_bShake = false;
	}

	// TODO : 플레이어의 좌표를 가져오게 되면 수정하기

	_vector vPos, vLook, vEyeResult, vAtResult;
	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	vEyeResult = vPos * 0.3f * vLook;
	vAtResult = vPos * vLook;

	_float4 f4Pos, f4Look, f4EyeResult, f4AtResult;
	XMStoreFloat4(&f4EyeResult, vEyeResult);
	XMStoreFloat4(&f4AtResult, vAtResult);

	m_CameraDesc.vEye.y += fRand;

	m_CameraDesc.vEye = f4EyeResult;
	m_CameraDesc.vAt = f4AtResult;
}

void CCamera_Dynamic::Key_Input(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(TimeDelta);
	}

	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(TimeDelta);
	}

	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}

	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}
	
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_RB) & 0x80)
	{
		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::ToFollow(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)	// 플레이어가 공격중인 상태에서는 움직이지 않는다.
	{
		if (pGameInstance->Key_Pressing(DIK_UP))
		{
			if (-12.1f < m_fYZ_Move)
				m_fYZ_Move -= _float(TimeDelta) * 0.1f;

			m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fYZ_Move);
		}
		if (pGameInstance->Key_Pressing(DIK_DOWN))
		{
			if (-11.9f > m_fYZ_Move)
				m_fYZ_Move += _float(TimeDelta) * 0.1f;

			m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fYZ_Move);
		}
	}

	_vector vPlayerPos, vTargetPos;
	vPlayerPos = CObj_Manager::GetInstance()->Get_Player_Transform();

	_float4 vf4TargetPos;
	XMStoreFloat4(&vf4TargetPos, vPlayerPos);
	vf4TargetPos = _float4(vf4TargetPos.x, vf4TargetPos.y + 3.7f, vf4TargetPos.z - 6.f, 1.f);
	vTargetPos = XMLoadFloat4(&vf4TargetPos);

	// 플레이어와의 거리가 일정거리 이상 멀어지게 되면 카메라는 가속을 받아 빠르게 플레이어에게 다가간다.

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos;											// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	if (7.f < fDistanceX || 6.f > fDistanceX)	// 빠르게 따라간다.		
		m_pTransformCom->Chase(vTargetPos, TimeDelta * 1.45);
	else										// 그냥 따라간다.
		m_pTransformCom->Chase(vTargetPos, TimeDelta);

	RELEASE_INSTANCE(CGameInstance);

	//CObj_Manager::PLAYERINFO	ePlayerInfo;
	//ePlayerInfo = CObj_Manager::GetInstance()->Get_Current_Player();
	//
	//if (ePlayerInfo.ePlayer == ePlayerInfo.FINN)
	//{
	//	// Finn 에게로
	//	CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), m_pTransformComTag, 0));

	//	_vector vPlayerPos, vTargetPos;
	//	vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	_float4 vf4TargetPos;
	//	XMStoreFloat4(&vf4TargetPos, vPlayerPos);
	//	vf4TargetPos = _float4(vf4TargetPos.x, vf4TargetPos.y + 3.7f, vf4TargetPos.z - 6.f, 1.f);
	//	vTargetPos = XMLoadFloat4(&vf4TargetPos);
	//	
	//	// 플레이어와의 거리가 일정거리 이상 멀어지게 되면 카메라는 가속을 받아 빠르게 플레이어에게 다가간다.

	//	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	//	_vector		vDir = vPlayerPos - vMyPos;											// 내 좌표가 객체를 바라보는 방향 벡터aaaaaa

	//	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));						// X 값을 뽑아와 거리 확인
	//	
	//	if (7.f < fDistanceX || 6.f > fDistanceX)		// 빠르게 따라간다. 9.17
	//		m_pTransformCom->Chase(vTargetPos, TimeDelta * 1.45); // ▤ : 1.4 가 아닌 자연스럽게 따라갈 수 있도록 수정
	//	else	// 그냥 따라간다.
	//		m_pTransformCom->Chase(vTargetPos, TimeDelta);
	//}
	//else if (ePlayerInfo.ePlayer == ePlayerInfo.JAKE)
	//{
	//	// Jaek 에게로
	//	CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), m_pTransformComTag, 0));

	//	_vector vPlayerPos, vTargetPos;
	//	vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	_float4 vf4TargetPos;
	//	XMStoreFloat4(&vf4TargetPos, vPlayerPos);
	//	vf4TargetPos = _float4(vf4TargetPos.x, vf4TargetPos.y + 3.7f, vf4TargetPos.z - 6.f, 1.f);
	//	vTargetPos = XMLoadFloat4(&vf4TargetPos);

	//	// 플레이어와의 거리가 일정거리 이상 멀어지게 되면 카메라는 가속을 받아 빠르게 플레이어에게 다가간다.

	//	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	//	_vector		vDir = vPlayerPos - vMyPos;											// 내 좌표가 객체를 바라보는 방향 벡터

	//	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인
	//	
	//	if (7.f < fDistanceX || 6.f > fDistanceX)	// 빠르게 따라간다.		
	//		m_pTransformCom->Chase(vTargetPos, TimeDelta * 1.45);
	//	else										// 그냥 따라간다.
	//		m_pTransformCom->Chase(vTargetPos, TimeDelta);

	//	//_vector vPlayerPos, vTargetPos;
	//	//vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	//vTargetPos = vPlayerPos + XMLoadFloat4(&m_vMinCamPos);
	//	//vTargetPos = XMVectorSetW(vTargetPos, 1.f);

	//	//// 플레이어와의 거리가 일정거리 이상 멀어지게 되면 카메라는 가속을 받아 빠르게 플레이어에게 다가간다.

	//	//_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);			// 내 좌표
	//	//_vector		vDir =  vPlayerPos - vMyPos;												// 내 좌표가 객체를 바라보는 방향 벡터
	//	//_vector		vMinDir = XMVectorSet(0.f, 1.f, 0.f, 1.f); // XMLoadFloat4(&m_vMinCamPos);	// 여기서는 실제 내 카메라의 좌표가 아닌, 임의의 축을 기준으로 계산하도록 한다.

	//	//_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));							// X 값을 뽑아와 거리 확인
	//	//_float		fMinCamPos = XMVectorGetX(XMVector3Length(vMinDir));

	//	//_float		fTemp = (fDistanceX - fMinCamPos) / (10.f - fMinCamPos);
	//	//if (1.f < fTemp)
	//	//	fTemp = 1.f;
	//	//else if (0.f > fTemp)
	//	//	fTemp *= -1;
	//	////cout << fTemp << endl;
	//	//m_fSpeed = 5.f * fTemp;	// 5.f PlayerSpeed

	//	//m_pTransformCom->Speed_Chase(vTargetPos, m_fSpeed, TimeDelta);
	//}
}

void CCamera_Dynamic::CurrentCamera_Tick(const _double & TimeDelta)
{
	switch (CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
	case CObj_Manager::PLAYERINFO::PLAYER::FINN:
		ToFollow(TimeDelta);
		break;
	case CObj_Manager::PLAYERINFO::PLAYER::JAKE:
		ToFollow(TimeDelta);
		break;
	case CObj_Manager::PLAYERINFO::PLAYER::FINNANDJAKE:
		FinnAndJake_Tick(TimeDelta);
		break;
	default:
	{
		if (LEVEL_GAMEPLAY == CObj_Manager::GetInstance()->Get_Current_Level())
			Action_Garden(TimeDelta);
		else
			Action_SkeletonBoss(TimeDelta);
	}
	break;
#ifdef F2_SKELETON
	case CObj_Manager::PLAYERINFO::PLAYER::FREE:
		Key_Input(TimeDelta);
		break;
#endif
	}
}

void CCamera_Dynamic::FinnAndJake_Tick(const _double & TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (CObj_Manager::PLAYERINFO::STATE::ATTACK != CObj_Manager::GetInstance()->Get_Current_Player().eState)	// 플레이어가 공격중인 상태에서는 움직이지 않는다.
	{
		if (pGameInstance->Key_Pressing(DIK_UP))
		{
			if (-12.1f < m_fYZ_Move)
				m_fYZ_Move -= _float(TimeDelta) * 0.1f;

			m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fYZ_Move);
		}
		if (pGameInstance->Key_Pressing(DIK_DOWN))
		{
			if (-11.9f > m_fYZ_Move)
				m_fYZ_Move += _float(TimeDelta) * 0.1f;

			m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fYZ_Move);
		}
	}

	CTransform * pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_MINIGAME, TEXT("Layer_FinnAndJake"), TEXT("Com_Transform"), 0));

	_vector vPlayerPos, vTargetPos;
	vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vf4TargetPos;
	XMStoreFloat4(&vf4TargetPos, vPlayerPos);
	vf4TargetPos = _float4(vf4TargetPos.x, vf4TargetPos.y + 3.7f, vf4TargetPos.z - 6.f, 1.f);
	vTargetPos = XMLoadFloat4(&vf4TargetPos);

	// 플레이어와의 거리가 일정거리 이상 멀어지게 되면 카메라는 가속을 받아 빠르게 플레이어에게 다가간다.

	_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vPlayerPos - vMyPos;											// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X 값을 뽑아와 거리 확인

	if (7.f < fDistanceX || 6.f > fDistanceX)	// 빠르게 따라간다.		
		m_pTransformCom->Chase(vTargetPos, TimeDelta * 1.45);
	else										// 그냥 따라간다.
		m_pTransformCom->Chase(vTargetPos, TimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::Action_Garden(const _double & TimeDelta)
{
	m_pTransformCom->Chase(XMVectorSet(-14.0277f, 3.7f, 42.1254f, 1.0f), TimeDelta * 2.0f);
}

void CCamera_Dynamic::Action_SkeletonBoss(const _double & TimeDelta)
{
	m_pTransformCom->Set_Pos(_float3(6.0f, 4.7f, 15.0f));
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();


}
