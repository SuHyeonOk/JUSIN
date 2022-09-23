#include "..\..\Header\PSystem.h"



CPSystem::CPSystem()
{
}


CPSystem::~CPSystem()
{
}

_bool CPSystem::init(IDirect3DDevice9 * pGraphicDev, const _tchar * texFileName)
{

	// D3D��ġ ������ �ʱ�ȭ �۾��� ó��
	FAILED_CHECK_RETURN(pGraphicDev->CreateVertexBuffer(
								m_vbSize * sizeof(PARTICLE),	// ���ؽ� ���� ũ��
								D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
								// _DYNAMIC : ���� ���ؽ����۸� ����Ѵ�.
								// _POINTS	: ���ؽ� ���۰� ����Ʈ ��������Ʈ�� ������ ������ ����
								FVF_PARTICLE,
								D3DPOOL_DEFAULT,
								&m_vb,
								0), FALSE);

	return TRUE;

	
}

void CPSystem::reset()
{
	// ��� ��ƼŬ �Ӽ��� ����
	for (auto& iter = m_particles.begin(); iter != m_particles.end(); ++iter)
	{
		resetParticle(&(*iter));
	}
}

void CPSystem::addParticle()
{
	// �ý��ۿ� ��ƼŬ�� �߰�.
	ATTRIBUTE tAttribute;

	resetParticle(&tAttribute);

	m_particles.push_back(tAttribute);
}

void CPSystem::preRender()
{
	// �������� �ռ� ������ �ʱ� ���� ����
	m_pGarphicDev->SetRenderState(D3DRS_LIGHTING,			FALSE);
	m_pGarphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE,	TRUE);
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALEENABLE,	TRUE);
	m_pGarphicDev->SetRenderState(D3DRS_POINTSIZE,			_ulong(m_fSize));
	m_pGarphicDev->SetRenderState(D3DRS_POINTSIZE_MIN,		_ulong(0.0f));
	
	// �Ÿ��� ���� ��ƼŬ ũ�� ����
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALE_A,		_ulong(0.0f));
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALE_B,		_ulong(0.0f));
	m_pGarphicDev->SetRenderState(D3DRS_POINTSCALE_C,		_ulong(1.0f));

	// �ؽ�ó�� ���ĸ� �̿��Ѵ�.
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
