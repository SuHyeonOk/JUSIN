#include "..\..\Header\PSystem.h"



CPSystem::CPSystem()
{
}


CPSystem::~CPSystem()
{
}

_bool CPSystem::init(IDirect3DDevice9 * pGraphicDev, const _tchar * texFileName)
{

	// D3D장치 의존적 초기화 작업을 처리
	FAILED_CHECK_RETURN(pGraphicDev->CreateVertexBuffer(
								m_vbSize * sizeof(PARTICLE),	// 버텍스 버퍼 크기
								D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
								// _DYNAMIC : 동적 버텍스버퍼를 사용한다.
								// _POINTS	: 버텍스 버퍼가 포인트 스프라이트를 보관할 것임을 지정
								FVF_PARTICLE,
								D3DPOOL_DEFAULT,
								&m_vb,
								0), FALSE);

	return TRUE;

	
}

void CPSystem::reset()
{
	// 모든 파티클 속성을 리셋
	for (auto& iter = m_particles.begin(); iter != m_particles.end(); ++iter)
	{
		resetParticle(&(*iter));
	}
}

void CPSystem::addParticle()
{
	// 시스템에 파티클을 추가.
	ATTRIBUTE tAttribute;

	resetParticle(&tAttribute);

	m_particles.push_back(tAttribute);
}

void CPSystem::preRender()
{
	// 렌더링에 앞서 지정할 초기 렌더 상태
	m_pGarphicDev->SetRenderState(D3DRS_LIGHTING,			FALSE);
	m_pGarphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE,	TRUE);
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALEENABLE,	TRUE);
	m_pGarphicDev->SetRenderState(D3DRS_POINTSIZE,			_ulong(m_fSize));
	m_pGarphicDev->SetRenderState(D3DRS_POINTSIZE_MIN,		_ulong(0.0f));
	
	// 거리에 따른 파티클 크기 제어
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALE_A,		_ulong(0.0f));
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALE_B,		_ulong(0.0f));
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALE_C,		_ulong(1.0f));

	// 텍스처의 알파를 이용한다.
	m_pGarphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

}

void CPSystem::render()
{
}

void CPSystem::postRender()
{
}

bool CPSystem::isEmpty()
{
	return false;
}

bool CPSystem::isDead()
{
	return false;
}

void CPSystem::removeDeadParticles()
{
}

void CPSystem::Free(void)
{
}
