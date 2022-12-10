#include "..\public\Transform.h"

#include "ImGuizmo.h"
#include "GameInstance.h"

#include "Shader.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
	
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

void CTransform::Go_Straight(_double TimeDelta)
{	
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);
	
	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Straight(_double TimeDelta, _float fSpeed)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vLook) * fSpeed * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Backward(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);

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

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scaled();
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;
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

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vTargetPos - vPosition;							// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));		// 그중 X 값을 뽑아와 거리 확인

	if (fDistance > fLimit)		// 만약 거리가 1 이고, 인자가 0.1 이면 따라간다. / 거리가 - 0 이고, 인자가 0.1 이면 따라가지 않는다.
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * _float(TimeDelta);
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

void CTransform::Speed_Chase(_fvector vTargetPos, _float fSpeed, _double TimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);	// 내 좌표
	_vector		vDir = vTargetPos - vPosition;							// 내 좌표가 객체를 바라보는 방향 벡터

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));		// 그중 X 값을 뽑아와 거리 확인

	if (fDistance > fLimit)		// 만약 거리가 1 이고, 인자가 0.1 이면 따라간다. / 거리가 - 0 이고, 인자가 0.1 이면 따라가지 않는다.
	{
		vPosition += XMVector3Normalize(vDir) * fSpeed * _float(TimeDelta);
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// sh

bool CTransform::Jump(_float fHeight, _float fSpeed, _double TimeDelta)
{
	// fHeight 까지 fSpeed 로 올라 갔다가 0에 닿으면 true 를 return 한다.

	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vUp = Get_State(CTransform::STATE_UP);

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

		if (0.05f >= f4Position.y) // 바닥에 파고 들어서 0.05 로 조정함 (바닥이 0이 아닌듯)
		{
			m_bJump = false;
			return true;
		}
	}

	return false;
}

void CTransform::RandomJump(_int iRandHeight, _float fSpeed, _float fminusHeight, _double TimeDelta, _bool bOneDir)
{
	// 랜덤 높이로 큰 점프 한 번, 작은 점프 3번을 하면서 랜덤 회전값 만큼 이동한다. (1번 실행)
	// 이 앞 내용이 다 끝나면 제자리에서 회전한다. (계속 실행)

	// (_float)(rand() % 1000) / 150.1
	_float fRandonHight = (_float)(rand() % iRandHeight) / 150.1f;

	if (!m_bBigJump) // 큰 점프
	{
		m_fSmallJump = 0.f;

		if (Jump(fRandonHight, fSpeed, TimeDelta))
			m_bBigJump = true;
	}
	else // 작은 점프
	{
		if (fRandonHight <= m_fSmallJump)
			m_bRotation = true; // 큰 점프 후 작은 점프 3번 후 회전

		if (Jump((fRandonHight - m_fSmallJump), (fSpeed + m_fSmallJump), TimeDelta))
			m_fSmallJump += fminusHeight;
	}

	if (!m_bRotation)
	{
		if (!m_bOneDir)
		{
			_float fRandonRot = (_float)(rand() % 360);
			Rotation(Get_State(CTransform::STATE_UP), XMConvertToRadians(fRandonRot));

			m_bOneDir = true;
		}
		Go_Straight(TimeDelta);
	}
	else
	{
		Set_Pos(0.f);
		Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), TimeDelta);

		return;
	}
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
