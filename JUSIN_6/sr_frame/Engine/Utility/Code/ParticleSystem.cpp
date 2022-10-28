#include "ParticleSystem.h"

#include "Export_Utility.h"
USING(Engine)

const DWORD VTXPARTICLE::ParticleFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

BoundingBox::BoundingBox()
{
	_min.x = FLT_MAX;
	_min.y = FLT_MAX;
	_min.z = FLT_MAX;

	_max.x = -FLT_MAX;
	_max.y = -FLT_MAX;
	_max.z = -FLT_MAX;
}

bool BoundingBox::IsPointInside(_vec3& _point)
{
	if (_point.x >= _min.x && _point.y >= _min.y && _point.z >= _min.z &&
		_point.x <= _max.x && _point.y <= _max.y && _point.z <= _max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
	, m_pVtxBf(nullptr)
	, m_pTexture(nullptr)
{
}

CParticleSystem::~CParticleSystem()
{
}


HRESULT CParticleSystem::Ready_Object(LPCWSTR _TexFileName)
{
	CGameObject::Ready_Object();

	HRESULT	hr = 0;
	hr = m_pGraphicDev->CreateVertexBuffer(
		m_dwVtxBf_Size * sizeof(VTXPARTICLE),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		tagParticle::ParticleFVF,
		D3DPOOL_DEFAULT,
		&m_pVtxBf,
		0);

	if (FAILED(hr))
	{
		MSG_BOX("CreateVertexBuffer() - FAILED");
		return E_FAIL;
	}

	hr = D3DXCreateTextureFromFile(
		m_pGraphicDev,
		_TexFileName,
		&m_pTexture);

	if (FAILED(hr))
	{
		MSG_BOX("D3DXCreateTextureFromFile() - FAILED");
		return E_FAIL;
	}

	return S_OK;
}

_int CParticleSystem::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);
	//Key_Input(fTimeDelta);
	Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CParticleSystem::Render_Object()
{
	CGameObject::Render_Object();

	if (!m_ParticlesList.empty())
	{
		PreRender_Particle();

		m_pGraphicDev->SetTexture(0, m_pTexture);
		m_pGraphicDev->SetFVF(tagParticle::ParticleFVF);
		m_pGraphicDev->SetStreamSource(0, m_pVtxBf, 0, sizeof(VTXPARTICLE));

		if (m_dwVtxBf_Offset >= m_dwVtxBf_Size)
			m_dwVtxBf_Offset = 0;

		VTXPARTICLE* v = 0;

		m_pVtxBf->Lock(
			m_dwVtxBf_Offset * sizeof(VTXPARTICLE),
			m_dwVtxBf_BatchSize * sizeof(VTXPARTICLE),
			(void**)&v,
			m_dwVtxBf_Offset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD dwNumParticlesInBatch = 0;

		std::list<Attribute>::iterator i;
		for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
		{
			if (i->_bIsAlive)
			{
 				v->_Pos = i->_vPosition;
				v->_Color = (D3DCOLOR)i->_color;
				v++; 

				dwNumParticlesInBatch++;

				if (dwNumParticlesInBatch == m_dwVtxBf_BatchSize)
				{
					m_pVtxBf->Unlock();

					m_pGraphicDev->DrawPrimitive(
						D3DPT_POINTLIST,
						m_dwVtxBf_Offset,
						m_dwVtxBf_BatchSize);

					m_dwVtxBf_Offset += m_dwVtxBf_BatchSize;

					if (m_dwVtxBf_Offset >= m_dwVtxBf_Size)
						m_dwVtxBf_Offset = 0;

					m_pVtxBf->Lock(
						m_dwVtxBf_Offset * sizeof(VTXPARTICLE),
						m_dwVtxBf_Size * sizeof(VTXPARTICLE),
						(void**)&v,
						m_dwVtxBf_Offset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					dwNumParticlesInBatch = 0; 
				}
			}
		}

		m_pVtxBf->Unlock();

		if (dwNumParticlesInBatch)
		{
			m_pGraphicDev->DrawPrimitive(
				D3DPT_POINTLIST,
				m_dwVtxBf_Offset,
				dwNumParticlesInBatch);
		}

		m_dwVtxBf_Offset += m_dwVtxBf_BatchSize;


		PostRender_Particle();
	}
}

void CParticleSystem::Free()
{
	m_ParticlesList.clear();
	Safe_Release(m_pVtxBf);
	Safe_Release(m_pTexture);
	CGameObject::Free();
}


void CParticleSystem::reset()
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		Reset_Particle(&(*i));
	}
}

void CParticleSystem::Add_Particle(const _vec3& _pos, const _float& _size, 
									const D3DXCOLOR& _color, const _uint& _numParticle, const _float& _lifetime, const _int& _type)
{
	Attribute attribute;
	attribute._vPosition = _pos;
	attribute._fSize = _size;
	attribute._color = _color;
	attribute._fLifeTime = _lifetime;
	attribute._iType = _type;

	for (_uint i = 0; i < _numParticle; ++i)
	{
		Reset_Particle(&attribute);
		m_ParticlesList.push_back(attribute);
	}

}

void CParticleSystem::PreRender_Particle()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &MAT_I);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// use alpha from texture
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CParticleSystem::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

bool CParticleSystem::IsEmpty()
{
	return m_ParticlesList.empty();
}

bool CParticleSystem::IsDead()
{
	std::list<Attribute>::iterator i;
	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
			return false;
	}

	return true;
}

void CParticleSystem::RemoveDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = m_ParticlesList.begin();

	while (i != m_ParticlesList.end())
	{
		if (i->_bIsAlive == false)
		{
			i = m_ParticlesList.erase(i);
		}
		else
		{
			i++;
		}
	}
}

float CParticleSystem::GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound)
		return lowBound;

	float f = (rand() % 10000) * 0.0001f;

	return (f * (highBound - lowBound)) + lowBound;
}

void CParticleSystem::GetRandomVector(_vec3* out, _vec3* min, _vec3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD CParticleSystem::FtoDw(float f)
{
	return *((DWORD*)&f);
}

D3DMATERIAL9 CParticleSystem::InitMtrl(D3DXCOLOR _ambient, D3DXCOLOR _diffuse, D3DXCOLOR _specular, D3DXCOLOR _emissive,
	float _power)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = _ambient;
	mtrl.Diffuse = _diffuse;
	mtrl.Specular = _specular;
	mtrl.Emissive = _emissive;
	mtrl.Power = _power;
	return mtrl;
}


