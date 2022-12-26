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

	m_fSpeed = m_CameraDesc.TransformDesc.fSpeedPerSec = 2.f;
	m_CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (m_eCameraInfo.eLevel == LEVEL_GAMEPLAY)
		m_CameraDesc.TransformDesc.f3Pos = _float3(m_eCameraInfo.f3Pos.x, m_eCameraInfo.f3Pos.y, m_eCameraInfo.f3Pos.z);
	if (m_eCameraInfo.eLevel == LEVEL_SKELETON)
		m_CameraDesc.TransformDesc.f3Pos = _float3(m_eCameraInfo.f3Pos.x, m_eCameraInfo.f3Pos.y, m_eCameraInfo.f3Pos.z);

	if (FAILED(CCamera::Initialize(&m_CameraDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_pTransformCom->Set_Pos();

	m_vMinCamPos = _float4(0.f, 4.5f, -5.f, 1.f);

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	if(m_eCameraInfo.eLevel != LEVEL_TOOL)
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

	CObj_Manager::PLAYERINFO	ePlayerInfo;
	ePlayerInfo = CObj_Manager::GetInstance()->Get_Current_Player();
	
	if (ePlayerInfo.ePlayer == ePlayerInfo.FINN)
	{
		// Finn ���Է�
		CTransform * pFinnTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Finn"), m_pTransformComTag, 0));

		_vector vPlayerPos, vTargetPos;
		vPlayerPos = pFinnTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float4 vf4TargetPos;
		XMStoreFloat4(&vf4TargetPos, vPlayerPos);
		vf4TargetPos = _float4(vf4TargetPos.x, vf4TargetPos.y + 4.5f, vf4TargetPos.z - 5.f, 1.f);
		vTargetPos = XMLoadFloat4(&vf4TargetPos);
		
		// �÷��̾���� �Ÿ��� �����Ÿ� �̻� �־����� �Ǹ� ī�޶�� ������ �޾� ������ �÷��̾�� �ٰ�����.

		_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
		_vector		vDir = vPlayerPos - vMyPos;											// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����aaaaaa

		_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));						// X ���� �̾ƿ� �Ÿ� Ȯ��
		
		if (7.f < fDistanceX || 6.f > fDistanceX)		// ������ ���󰣴�. 9.17
			m_pTransformCom->Chase(vTargetPos, TimeDelta * 1.45); // �� : 1.4 �� �ƴ� �ڿ������� ���� �� �ֵ��� ����
		else	// �׳� ���󰣴�.
			m_pTransformCom->Chase(vTargetPos, TimeDelta);
	}
	else if (ePlayerInfo.ePlayer == ePlayerInfo.JAKE)
	{
		// Jaek ���Է�
		CTransform * pJakeTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_Jake"), m_pTransformComTag, 0));

		_vector vPlayerPos, vTargetPos;
		vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_float4 vf4TargetPos;
		XMStoreFloat4(&vf4TargetPos, vPlayerPos);
		vf4TargetPos = _float4(vf4TargetPos.x, vf4TargetPos.y + 4.7f, vf4TargetPos.z - 5.f, 1.f);
		vTargetPos = XMLoadFloat4(&vf4TargetPos);

		// �÷��̾���� �Ÿ��� �����Ÿ� �̻� �־����� �Ǹ� ī�޶�� ������ �޾� ������ �÷��̾�� �ٰ�����.

		_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
		_vector		vDir = vPlayerPos - vMyPos;											// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

		_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));					// X ���� �̾ƿ� �Ÿ� Ȯ��
		
		if (7.f < fDistanceX || 6.2f > fDistanceX)	// ������ ���󰣴�.		
			m_pTransformCom->Chase(vTargetPos, TimeDelta * 1.45);
		else										// �׳� ���󰣴�.
			m_pTransformCom->Chase(vTargetPos, TimeDelta);

		//_vector vPlayerPos, vTargetPos;
		//vPlayerPos = pJakeTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		//vTargetPos = vPlayerPos + XMLoadFloat4(&m_vMinCamPos);
		//vTargetPos = XMVectorSetW(vTargetPos, 1.f);

		//// �÷��̾���� �Ÿ��� �����Ÿ� �̻� �־����� �Ǹ� ī�޶�� ������ �޾� ������ �÷��̾�� �ٰ�����.

		//_vector		vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);			// �� ��ǥ
		//_vector		vDir =  vPlayerPos - vMyPos;												// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����
		//_vector		vMinDir = XMVectorSet(0.f, 1.f, 0.f, 1.f); // XMLoadFloat4(&m_vMinCamPos);	// ���⼭�� ���� �� ī�޶��� ��ǥ�� �ƴ�, ������ ���� �������� ����ϵ��� �Ѵ�.

		//_float		fDistanceX = XMVectorGetX(XMVector3Length(vDir));							// X ���� �̾ƿ� �Ÿ� Ȯ��
		//_float		fMinCamPos = XMVectorGetX(XMVector3Length(vMinDir));

		//_float		fTemp = (fDistanceX - fMinCamPos) / (10.f - fMinCamPos);
		//if (1.f < fTemp)
		//	fTemp = 1.f;
		//else if (0.f > fTemp)
		//	fTemp *= -1;
		////cout << fTemp << endl;
		//m_fSpeed = 5.f * fTemp;	// 5.f PlayerSpeed

		//m_pTransformCom->Speed_Chase(vTargetPos, m_fSpeed, TimeDelta);
	}
	else if (ePlayerInfo.ePlayer == ePlayerInfo.FREE)
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
		m_CameraDesc.vAt.y += fRand;
	}
	else
	{
		m_dShakeTimeNow = 0;
		m_bShake = false;
	}

	// TODO : �÷��̾��� ��ǥ�� �������� �Ǹ� �����ϱ�

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
