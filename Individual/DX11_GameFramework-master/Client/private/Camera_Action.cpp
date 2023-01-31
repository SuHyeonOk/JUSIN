	#include "stdafx.h"
#include "..\public\Camera_Action.h"

#include "GameInstance.h"

#include "Obj_Manager.h"

CCamera_Action::CCamera_Action(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Action::CCamera_Action(const CCamera_Action & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Action::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCamera_Action::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_eCameraInfo, pArg, sizeof(CAMERAINFO));

	ZeroMemory(&m_CameraDesc, sizeof m_CameraDesc);

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(m_CameraDesc));

	m_CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	m_CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	m_CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	m_fSpeed = m_CameraDesc.TransformDesc.fSpeedPerSec = 2.f;
	m_CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	m_CameraDesc.TransformDesc.f3Pos = _float3(m_eCameraInfo.f3Pos.x, m_eCameraInfo.f3Pos.y, m_eCameraInfo.f3Pos.z);

	if (FAILED(CCamera::Initialize(&m_CameraDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_pTransformCom->Set_Pos();
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fYZ_Move);

	m_vMinCamPos = _float4(0.f, 4.5f, -5.f, 1.f);

	return S_OK;
}

void CCamera_Action::Tick(_double TimeDelta)
{


	ToFollow(TimeDelta);

	//if (pGameInstance->Key_Down(DIK_U))
	//{
	//	Shake_Camera(1, 2);
	//}

	//if(m_bShake)
	//	Shake_Camera(TimeDelta);
	//
	////m_pTransformCom->LookAt()

	__super::Tick(TimeDelta);
}

void CCamera_Action::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);	

	
}

HRESULT CCamera_Action::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Action::SetUp_Components()
{

	return S_OK;
}

void CCamera_Action::Key_Input(_double TimeDelta)
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

void CCamera_Action::ToFollow(_double TimeDelta)
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

	if (CObj_Manager::PLAYERINFO::PLAYER::FINN == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer ||
		CObj_Manager::PLAYERINFO::PLAYER::JAKE == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer ||
		CObj_Manager::PLAYERINFO::PLAYER::RESET == CObj_Manager::GetInstance()->Get_Current_Player().ePlayer)
	{
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
	}
#ifdef F2_SKELETON
	else
	{
		Key_Input(TimeDelta);
	}
#endif

	RELEASE_INSTANCE(CGameInstance);
}

CCamera_Action * CCamera_Action::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CCamera_Action*		pInstance = new CCamera_Action(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Action");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Action::Clone(void * pArg)
{
	CCamera_Action*		pInstance = new CCamera_Action(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Action");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Action::Free()
{
	__super::Free();


}
