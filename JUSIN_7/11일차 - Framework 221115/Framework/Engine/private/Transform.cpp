#include "..\public\Transform.h"
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

void CTransform::Set_Scaled(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return; // 예외 처리

	// 내가 계산할 State 를 얻음
	_vector		vState = Get_State(eState);

	// 상수로 만들고 * 원하는 길이를 곱 = 내가 원하는 길이
	vState = XMVector3Normalize(vState) * fScale;

	// 적용
	Set_State(eState, vState);
}

void CTransform::Set_Scaled(_float3 vScale)
{
	// 한 번에 적용 (해당 State에, 상수로 만들고 * 내가 원하는 길이 곱) = 내가 원하는 길이
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return; // 예외저리

	// 한 번에 적용 (해당 State에, 현재 State 안의 값 * 내가 배수 할 값)
	// = fScale 만큼 배수 된 값
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

void CTransform::Go_Straight(_double TimeDelta)
{	
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 vLOOK은 Z축 스케일을 포함한다. */
	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Backward(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 vLOOK은 Z축 스케일을 포함한다. */
	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 vLOOK은 Z축 스케일을 포함한다. */
	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 vLOOK은 Z축 스케일을 포함한다. */
	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * TimeDelta);

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);	

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK , XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	// TODO : 선생님이 회전시의 Scale 값을 넣어주시지 않았음
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scaled().x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scaled().y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scaled().z;

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

	Set_State(CTransform::STATE_RIGHT, vLook);
	Set_State(CTransform::STATE_UP, vRight);
	Set_State(CTransform::STATE_LOOK, vUp);
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit)
{
	// 내 Pos 가져오기
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	// 갈 pos - 내 pos = 방향 벡터
	_vector		vDir = vTargetPos - vPosition;

	// 방향 벡터를 길이로 변경
	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	// fLimit 보다 클 때만 따라온다 (fLimit default = 0.1.f)
	if(fDistance > fLimit)
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * TimeDelta;	
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName)
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
