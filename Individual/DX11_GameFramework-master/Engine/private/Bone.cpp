#include "..\public\Bone.h"



CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone* pParent)
{
	strcpy_s(m_szName, pAINode->mName.data); // 이름 wjwkd

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity()); // 당장은 가져오지 못 해서 항등

	memcpy(&m_TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4)); // 부모 노드 기준으로 표현되어 있는 상태 변환 행렬

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix))); // 헹렬 전치해서 사용하기!! 잊지말자!

	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity()); // 부모를 셋팅한 후에 다시!

	m_pParent = pParent;

	Safe_AddRef(m_pParent);

	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;

	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix)
			* XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));

}

CBone * CBone::Create(aiNode * pAINode, CBone* pParent)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);

}
