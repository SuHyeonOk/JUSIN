#include "..\..\Header\Transform.h"

#include "Export_Function.h"

USING(Engine)

CTransform::CTransform()
	: m_vScale(1.f, 1.f, 1.f)
	, m_vAngle(0.f, 0.f, 0.f)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matWorld);
}


Engine::CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs),m_vScale(rhs.m_vScale), m_vAngle(rhs.m_vAngle)
{
	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(m_vInfo[i], rhs.m_vInfo[i], sizeof(_vec3));

	memcpy(m_matWorld, rhs.m_matWorld, sizeof(_matrix));
	


}

CTransform::~CTransform()
{
}

void Engine::CTransform::Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta)
{
	_vec3		vDir = *pTargetPos - m_vInfo[INFO_POS];

	m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;

	_matrix		matRot = *Compute_LookAtTarget(pTargetPos);
	_matrix		matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POS].x, m_vInfo[INFO_POS].y, m_vInfo[INFO_POS].z);

	m_matWorld = matScale * matRot * matTrans;
}

const _matrix* Engine::CTransform::Compute_LookAtTarget(const _vec3* pTargetPos)
{
	_vec3		vLook = *pTargetPos - m_vInfo[INFO_POS];

	_vec3		vAxis, vUp;
	_matrix		matRot;
	
	// D3DXMatrixRotationAxis : 임의의 축회전 행렬을 만들어주는 함수
	return D3DXMatrixRotationAxis(&matRot, 
									D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vLook),
									acosf(D3DXVec3Dot(D3DXVec3Normalize(&vLook, &vLook), 
												D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]))));
}

HRESULT CTransform::Ready_Transform(void)
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
	
	return S_OK;
}

_int CTransform::Update_Component(const _float & fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);

	// 크기

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
	}

	for(_uint i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}
	
	// 회전
	_matrix		matRot[ROT_END];
	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		for (_uint j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	
	for (_uint i = 0; i < INFO_END; ++i)
	{
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));
	}
	
	return 0;
}

CTransform * CTransform::Create(void)
{
	CTransform* pInstance = new CTransform;
	if (FAILED(pInstance->Ready_Transform()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent* Engine::CTransform::Clone(void)
{
	return new CTransform(*this);
}

void CTransform::Free(void)
{
	CComponent::Free();
}
