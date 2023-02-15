#include "..\public\Transform.h"

#include "ImGuizmo.h"
#include "GameInstance.h"

#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
	
}

void CTransform::Set_State(STATE eState, _fvector vState, CNavigation * pNaviCom)
{
	_float4		vTmp;
	XMStoreFloat4(&vTmp, vState);

	if (nullptr == pNaviCom)	// �׺� �� ���� ��ü
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);

	else	 // �׺� ���� ��ü
	{
		pNaviCom->isMove_OnNavigation(vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);
	}
}

void CTransform::Set_Pos()
{
	m_WorldMatrix.m[STATE_TRANSLATION][0] = m_TransformDesc.f3Pos.x;
	m_WorldMatrix.m[STATE_TRANSLATION][1] = m_TransformDesc.f3Pos.y;
	m_WorldMatrix.m[STATE_TRANSLATION][2] = m_TransformDesc.f3Pos.z;
}

void CTransform::Set_Scaled(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	_vector		vState = Get_State(eState);

	vState = XMVector3Normalize(vState) * fScale;

	Set_State(eState, vState);
}

void CTransform::Set_Scaled(_float3 vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	Set_State(eState, Get_State(eState) * fScale);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Imgui_RenderProperty()
{
	ImGuizmo::BeginFrame();
	static float snap[3] = {1.f, 1.f, 1.f};
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m_WorldMatrix), matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Translate", matrixTranslation);
	ImGui::InputFloat3("Rotate", matrixRotation);
	ImGui::InputFloat3("Scale", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&m_WorldMatrix));

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("##something", &useSnap);
	ImGui::SameLine();
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}
	
	ImGuiIO& io = ImGui::GetIO();
	RECT rt;
	GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
	POINT lt{rt.left, rt.top};
	ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
	ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4 matView, matProj;
	XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	
	ImGuizmo::Manipulate(
		reinterpret_cast<float*>(&matView), 
		reinterpret_cast<float*>(&matProj),
		mCurrentGizmoOperation, 
		mCurrentGizmoMode, 
		reinterpret_cast<float*>(&m_WorldMatrix),
		nullptr, useSnap ? &snap[0] : nullptr);
}

void CTransform::Go_Straight(_double TimeDelta, CNavigation* pNaviCom)
{	
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* �̷��� ���� VlOOK�� Z�� �������� �����Ѵ�. */
	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	if (nullptr == pNaviCom)	// �׺� �� ���� ��ü
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

	else	 // �׺� ���� ��ü
	{
		if (true == pNaviCom->isMove_OnNavigation(vPosition))
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

void CTransform::Go_Straight(_double TimeDelta, _float fSpeed, CNavigation* pNaviCom)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* �̷��� ���� VlOOK�� Z�� �������� �����Ѵ�. */
	vPosition += XMVector3Normalize(vLook) * fSpeed * _float(TimeDelta);

	if (nullptr == pNaviCom)	// �׺� �� ���� ��ü
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

	else	 // �׺� ���� ��ü
	{
		if (true == pNaviCom->isMove_OnNavigation(vPosition))
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

void CTransform::Go_Backward(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	if (nullptr == pNaviCom)	// �׺� �� ���� ��ü
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

	else	 // �׺� ���� ��ü
	{
		if (true == pNaviCom->isMove_OnNavigation(vPosition))
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* �̷��� ���� VlOOK�� Z�� �������� �����ϳ�. */
	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Up(const _double & TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Down(const _double & TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * _float(TimeDelta));

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);	

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK , XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian) //�� ���� ���� �������� ���� �Է��� ������ŭȸ�� ��Ű�� �Լ�
{
	_float3		vScale = Get_Scaled();	// �������� �����´�.
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);	// ������ ���� �������� ȸ����Ų ���

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;	// Right ���Ϳ� �������� �����ϱ� ���ؼ� x, y, z, w �� �������� ���Ѵ�.
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;		// Up ���Ϳ� �������� �����ϱ� ���ؼ� x, y, z, w �� �������� ���Ѵ�.
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;		// Look ���Ϳ� �������� �����ϱ� ���ؼ� x, y, z, w �� �������� ���Ѵ�.

	// XMVector4Transform() : �� �Լ��� �𸣰ڴ�.
	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));	// Set_State() �� 1���ڷ� ���°�� �ش��ϴ� �������� �ְ�, 2���ڿ� ���͸� �־��ָ� �� ������ �ش� ����� ���Ͱ��� ����ȴ�.
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vTargetPos, _bool bIsInv)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook;

	if(bIsInv)
		vLook = -1.f * XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;
	else
		vLook = XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::LookAtForLandObject(_fvector vTargetPos)
{
	_float3 vSclae = Get_Scaled();
	_vector vPosition = Get_State(CTransform::STATE_TRANSLATION);

	_vector vLook = vTargetPos - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vLook = XMVector3Cross(vRight, vUp);

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vSclae.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vSclae.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vSclae.z);
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit, class CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	_vector		vDir = vTargetPos - vPosition;							// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));		// ���� X ���� �̾ƿ� �Ÿ� Ȯ��

	if (nullptr == pNaviCom)	// �׺� �� ���� ��ü
	{
		if (fDistance > fLimit)		// ���� �Ÿ��� 1 �̰�, ���ڰ� 0.1 �̸� ���󰣴�. / �Ÿ��� - 0 �̰�, ���ڰ� 0.1 �̸� ������ �ʴ´�.
		{
			vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
		}
	}
	else						// �׺� ���� ��ü
	{
		if (true == pNaviCom->isMove_OnNavigation(vPosition))
		{
			if (fDistance > fLimit)		// ���� �Ÿ��� 1 �̰�, ���ڰ� 0.1 �̸� ���󰣴�. / �Ÿ��� - 0 �̰�, ���ڰ� 0.1 �̸� ������ �ʴ´�.
			{
				vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);
				Set_State(CTransform::STATE_TRANSLATION, vPosition);
			}
		}
	}
}

void CTransform::Speed_Chase(_fvector vTargetPos, _float fSpeed, _double TimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	_vector		vDir = vTargetPos - vPosition;							// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));		// ���� X ���� �̾ƿ� �Ÿ� Ȯ��

	if (fDistance > fLimit)		// ���� �Ÿ��� 1 �̰�, ���ڰ� 0.1 �̸� ���󰣴�. / �Ÿ��� - 0 �̰�, ���ڰ� 0.1 �̸� ������ �ʴ´�.
	{
		vPosition += XMVector3Normalize(vDir) * fSpeed * _float(TimeDelta);
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

_bool CTransform::TargetPoint(_fvector vTargetPos, _double TimeDelta)	// ��ǥ������ �����ϸ� ���߰�, true �� ��ȯ���ִ� �Լ�
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);	// �� ��ǥ
	_vector		vDir = vTargetPos - vPosition;							// �� ��ǥ�� ��ü�� �ٶ󺸴� ���� ����

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));		// ���� X ���� �̾ƿ� �Ÿ� Ȯ��

	if (0.1f > fDistance)
	{
		return true;
	}
	else
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// sh

bool CTransform::Jump(_float fHeight, _float fSpeed, _double TimeDelta)
{
	// fHeight ���� fSpeed �� �ö� ���ٰ� 0�� ������ true �� return �Ѵ�.

	_vector	vUp = Get_State(CTransform::STATE_UP);
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);

	_float4 f4Position;
	XMStoreFloat4(&f4Position, vPosition);

	if (fHeight >= f4Position.y && !m_bJump)
	{
		vPosition += XMVector3Normalize(vUp) * fSpeed * _float(TimeDelta);
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
	else
	{
		m_bJump = true;
		vPosition -= XMVector3Normalize(vUp) * fSpeed * _float(TimeDelta);
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

		if (0.05f >= f4Position.y) // �ٴڿ� �İ� �� 0.05 �� ������ (�ٴ��� 0�� �ƴѵ�)
		{
			m_bJump = false;
			return true;
		}
	}

	return false;
}

//void CTransform::RandomJump(_float iRandHeight, _float fSpeed, _float fminusHeight, _double TimeDelta, _bool bOneDir, _bool bTurn)
//{
//	// ���� ���̷� ū ���� �� ��, ���� ���� 3���� �ϸ鼭 ���� ȸ���� ��ŭ �̵��Ѵ�. (1�� ����)
//	// �� �� ������ �� ������ ���ڸ����� ȸ���Ѵ�. (��� ����)
//
//	// (_float)(rand() % 1000) / 150.1
//	//_float fRandonHight = (_float)(rand() % iRandHeight) / 150.1f;
//
//	_float fRandonHight	= iRandHeight;
//
//	if (!m_bBigJump) // ū ����
//	{
//		m_fSmallJump = 0.f;
//
//		if (Jump(fRandonHight, fSpeed, TimeDelta))
//			m_bBigJump = true;
//	}
//	else // ���� ����
//	{
//		if (fRandonHight <= m_fSmallJump)
//			m_bRotation = true; // ū ���� �� ���� ���� 3�� �� ȸ��
//
//		if (Jump((fRandonHight - m_fSmallJump), (fSpeed + m_fSmallJump), TimeDelta))
//			m_fSmallJump += fminusHeight;
//	}
//
//	if (!m_bRotation)
//	{
//		if (!m_bOneDir)
//		{
//			m_bOneDir = true;
//
//			_float fRandonRot = (_float)(rand() % 360);
//			Rotation(Get_State(CTransform::STATE_UP), XMConvertToRadians(fRandonRot));
//		}
//		Go_Straight(TimeDelta);
//	}
//	else
//	{
//		Set_Pos(0.f);
//
//		if (false == bTurn)
//			return;
//		
//		Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);
//	}
//}

_vector CTransform::LinearVector(_fvector vSour, _fvector vDest, _float fAngle)
{
	// XMVector3Normalize : ���� ���� �̱� ������ Normalize �� �Ѵ�.
	// XMVectorSetW : ���� �����̱� ������ w ���� 0.0f �� �ʱ�ȭ ���ش�.
	return XMVectorSetW(XMVector3Normalize(vSour * (1 - fAngle) + (vDest * fAngle)), 0.0f);
}

void CTransform::PlayerMove(_fvector vNewLook, const _double & TimeDelta)
{
	// ��� ������ ������ �ϰ� ����� ���� Look �� ����� �� ������ �����Ѵ�.
	_vector vCurrentLook = Get_State(CTransform::STATE_LOOK);

	_float fArithmetic = XMVectorGetX(XMVector3Dot(vCurrentLook, vNewLook));
	
	if (1.0f != fArithmetic)
	{
		if (1 > m_fAngle)
			m_fAngle += _float(TimeDelta);
	}
	else //if (1.0f == fArithmetic)
	{
		m_fAngle = 0;
	}

	if (-1.0f == fArithmetic)
	{
		Turn({ 0.0f, 1.0f, 0.0f, 1.0f }, TimeDelta * 10.0);
	}
	else
	{
		// LinearVector() �Լ��� �̿��Ͽ� ���� �ٶ���� �ϴ� Look �� ���� ���� m_fAngle �� ���� õõ�� �޾ƿ´�.
		_vector vTempLook = LinearVector(vCurrentLook, vNewLook, m_fAngle);
		_vector vTempRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vTempLook);
		_vector vTempUp = XMVector3Cross(vTempLook, vTempRight);

		Set_State(CTransform::STATE_RIGHT, vTempRight);
		Set_State(CTransform::STATE_UP, vTempUp);
		Set_State(CTransform::STATE_LOOK, vTempLook);
	}
}

_bool CTransform::Go_SwinUp(_double TimeDelta, _float fSpeed)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * fSpeed * _float(TimeDelta);

	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vPosition);

	if (f4MyPos.y >= 0)
	{
		Set_Pos(0.f);
		return true;
	}

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
	return false;
}

void CTransform::Go_SwinDown(_double TimeDelta, _float fDeep)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * _float(TimeDelta);

	_float4 f4MyPos;
	XMStoreFloat4(&f4MyPos, vPosition);

	if (f4MyPos.y <= fDeep)
		return;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShaderCom, const char* pConstantName)
{
	if (nullptr == pShaderCom)
		return E_FAIL;

	return pShaderCom->Set_Matrix(pConstantName, &m_WorldMatrix);	
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();


}
