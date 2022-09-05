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
	// 벡터들 한테 월드 행렬의 값을 곱해준다
	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(m_vInfo[i], rhs.m_vInfo[i], sizeof(_vec3));

	memcpy(m_matWorld, rhs.m_matWorld, sizeof(_matrix));
	


}

CTransform::~CTransform()
{
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
	// [월드 행렬 만들기]

	// 기존행렬은 항등행렬로 바꾸어 재구성 한다
	D3DXMatrixIdentity(&m_matWorld);

	// 크기
	// ﻿일단 위치는 제외한다
	// ﻿크기와 회전이 적용되는 것은 11~33까지의 9개 원소지 41, 42, 43에는 크기, 회전값이 적용되지 않기에 3개의 벡터만 적용한다
	for (_uint i = 0; i < INFO_POS; ++i)
	{
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
	}

	// 크기 값 지정
	for(_uint i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]); // 단위벡터로 만들고
		m_vInfo[i] *= *(((_float*)&m_vScale) + i); // m_vScale을 곱해주는데
		// 벡터끼리는 곱셈을 할 수 없어서 주소 값으로 만들고
		// float으로 형 변환을하여 i로 포인터 연산을 하면
		// 4byte단위로 움직일 수 있다 이 값을 m_vInfo[i]에 곱한다

		// 다시 벡터의 주소를 float로 바꾸고 4byte씩 +i를 해서 이동하면
		// x, y, z 메모리 공간에 접근 할 수 있다

		// 👉 float 값을 Vector에 곱하는 방법
	}
	
	// 회전
	_matrix		matRot[ROT_END];
	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	// 벡터 3개, 행렬 3개 총 9번 곱하기
	for (_uint i = 0; i < INFO_POS; ++i)
	{
		for (_uint j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	// 위치X
	// 위치의 경우 Set_Pos를 만들어서 x,y,z에 직접 대입하기 때문에 여기서 하지는 않는다
	// Set_Pos는 Update에서 Key입력 처럼 이벤트가 발생하면 입력 받도록 하면 된다

	// 변환이 끝난 벡터로 월드 행렬을 구성하는 코드
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
