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
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

		CameraDesc.TransformDesc.fSpeedPerSec = 3.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}
	
	if (FAILED(CCamera::Initialize(&CameraDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
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

	CObj_Manager::PLAYER	ePlayer;
	ePlayer = CObj_Manager::GetInstance()->Get_Current_Player();

	if (ePlayer == CObj_Manager::FINN)
	{

		// Finnxx 에게로
		CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Finn"), m_pTransformComTag, 0));

		_vector vPlayerPos;
		vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float4 vf4PlayerPos;
		XMStoreFloat4(&vf4PlayerPos, vPlayerPos);

		m_pTransformCom->Set_Pos(_float3(vf4PlayerPos.x, vf4PlayerPos.y + 6.f, vf4PlayerPos.z - 7.f));
	}
	else if (ePlayer == CObj_Manager::JAKE)
	{

		// Jaek 에게로
		CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Jake"), m_pTransformComTag, 0));

		_vector vPlayerPos;
		vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float4 vf4PlayerPos;
		XMStoreFloat4(&vf4PlayerPos, vPlayerPos);

		m_pTransformCom->Set_Pos(_float3(vf4PlayerPos.x, vf4PlayerPos.y + 5.f, vf4PlayerPos.z - 7.f));
	}
	else if (ePlayer == CObj_Manager::FREE)
	{
		Key_Input(TimeDelta);
	}

	RELEASE_INSTANCE(CGameInstance);
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
		CameraDesc.vAt.y += fRand;
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

	CameraDesc.vEye.y += fRand;

	CameraDesc.vEye = f4EyeResult;
	CameraDesc.vAt = f4AtResult;
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
