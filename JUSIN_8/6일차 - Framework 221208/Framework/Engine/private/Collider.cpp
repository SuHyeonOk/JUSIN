#include "..\public\Collider.h"
#include "DebugDraw.h"

#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);	// 그리기 위해서
	m_pEffect->SetVertexColorEnabled(true);	// 색을 이용하기 위해서 (안 쓰면 텍스처)

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	COLLIDERDESC		ColliderDesc;
	memcpy(&ColliderDesc, pArg, sizeof(COLLIDERDESC));	

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		m_pAABB_Original->Transform(*m_pAABB_Original,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *				// 사이즈
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));	// 중심
		m_pAABB = new BoundingBox(*m_pAABB_Original);
		break;

	case CCollider::TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

		m_pOBB->Transform(*m_pOBB,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *
			XMMatrixRotationX(ColliderDesc.vRotation.x) *
			XMMatrixRotationY(ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(ColliderDesc.vRotation.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere = new BoundingSphere(_float3(0.f, 0.f, 0.f), ColliderDesc.vSize.x * 0.5f);

		m_pSphere->Transform(*m_pSphere,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *
			XMMatrixRotationX(ColliderDesc.vRotation.x) *
			XMMatrixRotationY(ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(ColliderDesc.vRotation.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));
		break;
	}
	
	return S_OK;
}
void CCollider::Update(_fmatrix TransformMatrix)
{
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB, TransformMatrix);		
		break;

	case CCollider::TYPE_OBB:

		break;

	case CCollider::TYPE_SPHERE:

		break;
	}
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{	
	m_vColor = m_isColl == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere);

#ifdef _DEBUG

	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif // _DEBUG

}
