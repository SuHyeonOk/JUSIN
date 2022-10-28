#include "stdafx.h"
#include "..\Header\Particle.h"
#include "Export_Utility.h"
#include "AbstFactory.h"
#include "RedStoneMonstrosity.h"
#include "SphereEffect.h"
#include "TerrainCubeMap.h"
#define			PI			3.141592f

#pragma region

#pragma endregion




#pragma region Particle System

#pragma region Attakc_Basic
CAttack_P::~CAttack_P() = default;

_int CAttack_P::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
			i->_fAge += fTimeDelta;

			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;
				
			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CAttack_P::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CAttack_P::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	//m_fSize = _Attribute->_fSize;

	m_fSize = GetRandomFloat(_Attribute->_fSize-0.1f,_Attribute->_fSize+0.1f);
	_vec3 min = _vec3(-3.0f, -3.0f, -3.0f);
	_vec3 max = _vec3(3.0f, 3.0f, 3.0f);

	GetRandomVector(
		&_Attribute->_vVelocity,
		&min,
		&max);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);
	_Attribute->_vVelocity *= 10.f;
	
	
	_Attribute->_fAge = 0.0f;
}

void CAttack_P::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	//m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	// m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CAttack_P::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CAttack_P* CAttack_P::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CAttack_P* Inst = new CAttack_P(pGraphicDev);

	Inst->m_dwVtxBf_Size = 2048;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 512;

	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CAttack_P::Free()
{
	CParticleSystem::Free();
}
#pragma endregion

#pragma region Attack_Range_Circle
CAttack_Range_Circle::~CAttack_Range_Circle()
{
}

HRESULT CAttack_Range_Circle::Ready_Object(const ATKRNGOPTION& circleOption)
{
	
	// m_pTransCom->m_vInfo[INFO_POS].y += 0.1f; // 위치 올려야됌

	m_ATKRNGOption._eRangeType = circleOption._eRangeType;
	m_ATKRNGOption._fAcc = circleOption._fAcc;
	m_ATKRNGOption._fMaxAcc = circleOption._fMaxAcc;
	m_ATKRNGOption._iTotalFrame = circleOption._iTotalFrame;
	m_ATKRNGOption._iNextFrame = circleOption._iNextFrame;
	m_ATKRNGOption._vMinSize = circleOption._vMinSize;
	m_ATKRNGOption._vMaxSize = circleOption._vMaxSize;
	m_ATKRNGOption._fLifeTime = circleOption._fLifeTime;
	m_ATKRNGOption._vLerpScale = CGameUtilMgr::s_vZero;
	// m_fTime = 0.f;
	m_fCurTime = 0.f;


	if (m_ATKRNGOption._eRangeType == READY_CIRCLE)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_Ready_Range_CircleCom", L"Proto_Ready_Range_CircleCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Attack_Circle", L"Proto_Attack_Circle", ID_STATIC);
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		m_pBufferCom->Set_TextureOption(0, 0, 0);
		m_pTransCom->m_vScale = m_ATKRNGOption._vMaxSize;
		m_fSpeed = 0.f; // 안씀
		tmp = 0.1f;
	}
	else if (m_ATKRNGOption._eRangeType == ATTACK_CIRCLE)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_Attack_Range_CircleCom", L"Proto_Attack_Range_CircleCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Attack_Circle", L"Proto_Attack_Circle", ID_STATIC);
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		m_pBufferCom->Set_TextureOption(0, 0, 0);
		m_pTransCom->m_vScale = m_ATKRNGOption._vMinSize;
		m_ATKRNGOption._vMaxSize *= 1.5f;
		tmp = 0.15f;
	}
	m_pTransCom->Update_Component(0.f);

	return S_OK;
}

_int CAttack_Range_Circle::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	vPos.y = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap")->GetHeight(vPos.x, vPos.z) + tmp;

	if (m_fCurTime >= m_ATKRNGOption._fLifeTime)
	{
		return OBJ_DEAD;
	}
	m_fCurTime += fTimeDelta;

	m_ATKRNGOption._fAcc += fTimeDelta;

	if (m_ATKRNGOption._eRangeType == READY_CIRCLE){}
	else if (m_ATKRNGOption._eRangeType == ATTACK_CIRCLE)
	{
		if(m_ATKRNGOption._fAcc > m_ATKRNGOption._fMaxAcc)
		{
			m_fCurTime = m_ATKRNGOption._fLifeTime;
		}
		else
		{
			D3DXVec3Lerp(&m_ATKRNGOption._vLerpScale, &m_ATKRNGOption._vMinSize, &m_ATKRNGOption._vMaxSize, m_ATKRNGOption._fAcc / m_ATKRNGOption._fMaxAcc);
			m_pTransCom->m_vScale = m_ATKRNGOption._vLerpScale;
		}
	}

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CAttack_Range_Circle::Render_Object()
{
	// CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CAttack_Range_Circle::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CAttack_Range_Circle::PreRender_Particle()
{
}

void CAttack_Range_Circle::PostRender_Particle()
{
}

void CAttack_Range_Circle::SetLerp(ATKRNGOPTION* _circleoption)
{
	

	// if (m_ATKRNGOption._eRangeType == READY_CIRCLE)
	// {
	// 	m_pTransCom->m_vScale *= 10.f;// m_ATKRNGOption._vMaxSize;
	// 	m_fSpeed = 0.f; // 안씀
	// 	m_fCurTime = 0.f;
	// }
	// else if (m_ATKRNGOption._eRangeType == ATTACK_CIRCLE)
	// {
	// 	m_pTransCom->m_vScale = m_ATKRNGOption._vMinSize;
	// 	m_fCurTime = 0.f;
	// }
	// m_pTransCom->Update_Component(0.f);
}

CAttack_Range_Circle* CAttack_Range_Circle::Create(LPDIRECT3DDEVICE9 pGraphicDev,const ATKRNGOPTION& circleOption)
{
	CAttack_Range_Circle* Inst = new CAttack_Range_Circle(pGraphicDev);

	if (FAILED(Inst->Ready_Object(circleOption)))
	{
		return nullptr;
	}
	return Inst;
}

void CAttack_Range_Circle::Free()
{
	CGameObject::Free();
}
#pragma endregion

#pragma region 3DBase
C3DBaseTexture::~C3DBaseTexture()
{
}

_int C3DBaseTexture::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
			i->_fAge += fTimeDelta;

			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;

			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void C3DBaseTexture::Render_Object()
{
	CParticleSystem::Render_Object();
}

void C3DBaseTexture::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	if(_Attribute->_iType == 1)
	{
		_Attribute->_vPosition.y +=  1.1f;
		_Attribute->_vVelocity = _vec3(0.f, EPSILON, 0.f);
		_Attribute->_vVelocity.y += EPSILON;
		
	}
	else
	{
		_Attribute->_vPosition.y += 4.f;
		_Attribute->_vVelocity = _vec3(0.f, 3.f, 0.f);

		D3DXVec3Normalize(
			&_Attribute->_vVelocity,
			&_Attribute->_vVelocity);

		// 터지는 방향 조절
		// _Attribute->_vVelocity.y = GetRandomFloat(0.0f, 1.0f) * 3.f;

		_Attribute->_vVelocity.y += 3.f;
	}
	_Attribute->_fAge = 0.0f;
}

void C3DBaseTexture::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void C3DBaseTexture::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

C3DBaseTexture* C3DBaseTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	C3DBaseTexture* Inst = new C3DBaseTexture(pGraphicDev);

	Inst->m_dwVtxBf_Size = 2048;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 512;

	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void C3DBaseTexture::Free()
{
	CParticleSystem::Free();
}
#pragma endregion

#pragma region SpeedBoots
CSpeedBoots::~CSpeedBoots()
{
}

_int CSpeedBoots::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
			i->_fAge += fTimeDelta;

			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;
			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CSpeedBoots::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CSpeedBoots::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;
	_Attribute->_vPosition.y += 4.f;
	_Attribute->_vVelocity = _vec3(0.f, 3.f, 0.f);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);

	// 터지는 방향 조절
	// _Attribute->_vVelocity.y = GetRandomFloat(0.0f, 1.0f) * 3.f;

	_Attribute->_vVelocity.y += 3.f;

	_Attribute->_fAge = 0.0f;
}

void CSpeedBoots::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CSpeedBoots::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CSpeedBoots* CSpeedBoots::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CSpeedBoots* Inst = new CSpeedBoots(pGraphicDev);

	Inst->m_dwVtxBf_Size = 2048;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 512;

	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CSpeedBoots::Free()
{
	CParticleSystem::Free();
}




#pragma endregion

#pragma region SpeedBoots_Particle

CSpeedBoots_Particle::~CSpeedBoots_Particle()
{
}

_int CSpeedBoots_Particle::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
			i->_fAge += fTimeDelta;

			if (_int(i->_fAge) % 2 == 0)
			{
				CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
				pPlayerTransform->Get_Info(INFO_POS, &i->_vPosition);
				i->_vPosition.y += 4.f;
			}

			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;

			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CSpeedBoots_Particle::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CSpeedBoots_Particle::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	pPlayerTransform->Get_Info(INFO_POS, &_Attribute->_vPosition);

	_Attribute->_vPosition.y += 4.f;

	_vec3 min = _vec3(0.0f, -1.0f, 0.0f);
	_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

	// m_tPosin.x = LONG(m_tInfo.fX + cosf(m_fAngle * (PI / 180.f)) * m_fDiagonal);
	// m_tPosin.y = LONG(m_tInfo.fY - sinf(m_fAngle * (PI / 180.f)) * m_fDiagonal);

	m_fRotAngle += GetRandomFloat(5.f, 10.f);

	_Attribute->_vVelocity.x += _Attribute->_vVelocity.x +  cosf(m_fRotAngle * (PI / 180.f));
	_Attribute->_vVelocity.y -= (_Attribute->_vVelocity.y) +   sinf(m_fRotAngle * (PI / 180.f));
	// _Attribute->_vVelocity.z = 0.f;

	_matrix matRot;
	D3DXMatrixRotationY(&matRot, D3DXToRadian(45.f));
	D3DXVec3TransformNormal(&_Attribute->_vVelocity, &_Attribute->_vVelocity, &matRot);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);

	_Attribute->_vVelocity *= m_fSpeed;
	// 터지는 방향 조절
	// _Attribute->_vVelocity.y = GetRandomFloat(0.0f, 1.0f) * 3.f;


	_Attribute->_fAge = 0.0f;
}

void CSpeedBoots_Particle::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CSpeedBoots_Particle::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CSpeedBoots_Particle* CSpeedBoots_Particle::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CSpeedBoots_Particle* Inst = new CSpeedBoots_Particle(pGraphicDev);

	Inst->m_dwVtxBf_Size = 2048;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 512;
	Inst->m_fSpeed = 5.f;
	Inst->m_fRotAngle = 0.f;
	Inst->m_fRotSpeed = 10.f;
	Inst->m_fDiagonal = 0.f;
	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CSpeedBoots_Particle::Free()
{
	CParticleSystem::Free();
}

#pragma endregion

#pragma region Firework_Fuze
CFireWork_Fuze::~CFireWork_Fuze()
{
}

_int CFireWork_Fuze::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
			i->_fAge += fTimeDelta;
			// CEffectFactory::Create<CLava_Particle>("Lava_Particle", L"Lava_Particle");

			if (i->_fAge > i->_fLifeTime)
			{
				vDeadPos = i->_vPosition;

				_float fColor = GetRandomFloat(0.0f, 1.0f);
				_float fBlend = GetRandomFloat(0.0f, 0.5f);
				_ULonglong tick = GetTickCount64();

				if(tick % 3 == 0)
					Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(vDeadPos, 0.5f, D3DXCOLOR(fColor, fBlend, 0, 0), 256, 0.5f);
				else if(tick % 3 == 1)
					Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(vDeadPos, 0.5f, D3DXCOLOR(0, fColor, fBlend, 0), 256, 0.5f);
				else if (tick % 3 == 2)
					Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(vDeadPos, 0.5f, D3DXCOLOR(fBlend, 0, fColor, 0), 256, 0.5f);
				else
					Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(vDeadPos, 0.5f, D3DXCOLOR(fColor, fBlend, fBlend, fColor), 256, 0.5f);

				 CEffectFactory::Create<CUVCircle>("Firwork_Circle", L"Firwork_Circle");

				i->_bIsAlive = false;
			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);


	return OBJ_NOEVENT;
}

void CFireWork_Fuze::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CFireWork_Fuze::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	pPlayerTransform->Get_Info(INFO_LOOK, &_Attribute->_vVelocity);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);

	_Attribute->_vVelocity *= 20.f;
	_Attribute->_fAge = 0.0f;
}

void CFireWork_Fuze::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFireWork_Fuze::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CFireWork_Fuze* CFireWork_Fuze::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CFireWork_Fuze* Inst = new CFireWork_Fuze(pGraphicDev);

	Inst->m_dwVtxBf_Size = 2048;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 512;
	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CFireWork_Fuze::Free()
{
	CParticleSystem::Free();
}

#pragma endregion

#pragma region FireWork
CFireWork::~CFireWork()
{
}

_int CFireWork::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
			i->_fAge += fTimeDelta;

			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;
			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CFireWork::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CFireWork::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	_vec3 min = _vec3(-1.0f, 0.0f, -1.0f);
	_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

	GetRandomVector(
		&_Attribute->_vVelocity,
		&min,
		&max);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);

		_Attribute->_vVelocity *= 10.f;

	_Attribute->_fAge = 0.0f;
}

void CFireWork::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFireWork::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CFireWork* CFireWork::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CFireWork* Inst = new CFireWork(pGraphicDev);
	Inst->Reset_tmp();
	Inst->m_dwVtxBf_Size = 4096 * 2;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 1024 * 2;
	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CFireWork::Free()
{
	CParticleSystem::Free();
}

#pragma endregion

CFireWork_Kouku::~CFireWork_Kouku()
{
}

_int CFireWork_Kouku::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelta;
	
			i->_fAge += fTimeDelta;

			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;
			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CFireWork_Kouku::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CFireWork_Kouku::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	_vec3 min = _vec3(-1.0f, 0.0f, -1.0f);
	_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

	GetRandomVector(
		&_Attribute->_vVelocity,
		&min,
		&max);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);

	_Attribute->_vVelocity *= 10.f;
	_Attribute->_vVelocity.y = 0.f;

	_Attribute->_fAge = 0.0f;
}

void CFireWork_Kouku::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFireWork_Kouku::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CFireWork_Kouku* CFireWork_Kouku::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CFireWork_Kouku* Inst = new CFireWork_Kouku(pGraphicDev);
	Inst->Reset_tmp();
	Inst->m_dwVtxBf_Size = 4096 * 2;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 1024 * 2;
	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CFireWork_Kouku::Free()
{
	CParticleSystem::Free();
}




#pragma region MoonParticle
CMoonParticle::~CMoonParticle()
{
}

_int CMoonParticle::Update_Object(const _float& fTimeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_fAge += fTimeDelta;

			CTransform*	pPlayerTransform = nullptr;
			for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second))
				{
					if (pPlayer->GetID() == 0)
					{
						pPlayerTransform = pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
						break;
					}
				}
			}

			_vec3 pPos;
			pPlayerTransform->Get_Info(INFO_POS, &pPos);
			if (i->_color == D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
			{
				i->_vPosition = pPos;
				i->_vPosition.y = pPos.y + 3.f;
			}
			else
			{
				i->_vPosition += i->_vVelocity * fTimeDelta;

				if(i->_vPosition.y >= pPos.y + 3.f)
				{
					i->_vPosition.y = pPos.y;
				}
			}
			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;
			}
		}
	}

	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CMoonParticle::Render_Object()
{
	CParticleSystem::Render_Object();


}

void CMoonParticle::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	

	if (_Attribute->_color == D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
	{

		CTransform*	pPlayerTransform = nullptr;
		for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second))
			{
				if (pPlayer->GetID() == 0)
				{
					pPlayerTransform = pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
					break;
				}
			}
		}
		_vec3 pPos;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);

		_vec3 min = _vec3(0.f, 0.0f, 0.f);
		_vec3 max = _vec3(0.f, 1.0f, 0.f);

		GetRandomVector(
			&_Attribute->_vVelocity,
			&min,
			&max);
		
		D3DXVec3Normalize(
			&_Attribute->_vVelocity,
			&_Attribute->_vVelocity);
		
		_Attribute->_vVelocity *= 0.1f;

	}
	else
	{
		_Attribute->_vPosition.x += CGameUtilMgr::GetRandomFloat(-0.25f, 0.25f);
		_Attribute->_vPosition.z += CGameUtilMgr::GetRandomFloat(-0.25f, 0.25f);

		_vec3 min = _vec3(0.f, 0.0f, 0.f);
		_vec3 max = _vec3(0.f, 1.0f, 0.f);

		GetRandomVector(
			&_Attribute->_vVelocity,
			&min,
			&max);

		D3DXVec3Normalize(
			&_Attribute->_vVelocity,
			&_Attribute->_vVelocity);

		_Attribute->_vVelocity *= CGameUtilMgr::GetRandomFloat(3.f, 6.f);
	}
	_Attribute->_fAge = 0.0f;
}

void CMoonParticle::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	// m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CMoonParticle::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CMoonParticle* CMoonParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CMoonParticle* Inst = new CMoonParticle(pGraphicDev);
	Inst->m_dwVtxBf_Size = 4096 * 2;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 1024 * 2;
	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CMoonParticle::Free()
{
	CParticleSystem::Free();
}
#pragma endregion

#pragma region Fascinate_Effect

CFascinated_Effect::~CFascinated_Effect()
{
}

_int CFascinated_Effect::Update_Object(const _float& fTimeDelta)
{

	std::list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
		{
			i->_fAge += fTimeDelta;

	/*		CTransform*	pPlayerTransform = nullptr;
			for (auto& e : Get_Layer(LAYER_PLAYER)->Get_MapObject())
			{
				if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(e.second))
				{
					if (pPlayer->GetID() == 0)
					{
						pPlayerTransform = pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
						break;
					}
				}
			}*/

			_vec3 pPos = i->_vPosition;
			i->_vPosition = pPos;
			i->_vPosition.y = pPos.y + 3.f;
			if (i->_fAge > i->_fLifeTime)
			{
				i->_bIsAlive = false;
			}
		}
	}
	RemoveDeadParticles();
	CParticleSystem::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;
}

void CFascinated_Effect::Render_Object()
{
	CParticleSystem::Render_Object();
}

void CFascinated_Effect::Reset_Particle(Attribute* _Attribute)
{
	_Attribute->_bIsAlive = true;
	m_fSize = _Attribute->_fSize;

	_vec3 min = _vec3(0.f, 0.0f, 0.f);
	_vec3 max = _vec3(0.f, 1.0f, 0.f);

	GetRandomVector(
		&_Attribute->_vVelocity,
		&min,
		&max);

	D3DXVec3Normalize(
		&_Attribute->_vVelocity,
		&_Attribute->_vVelocity);

	_Attribute->_vVelocity *= 0.1f;


	_Attribute->_fAge = 0.0f;
}

void CFascinated_Effect::PreRender_Particle()
{
	CParticleSystem::PreRender_Particle();
	// m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	// m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFascinated_Effect::PostRender_Particle()
{
	CParticleSystem::PostRender_Particle();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CFascinated_Effect* CFascinated_Effect::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName)
{
	CFascinated_Effect* Inst = new CFascinated_Effect(pGraphicDev);
	Inst->m_dwVtxBf_Size = 4096 * 2;
	Inst->m_dwVtxBf_Offset = 0;
	Inst->m_dwVtxBf_BatchSize = 1024 * 2;
	if (FAILED(Inst->Ready_Object(_TexFileName)))
	{
		return nullptr;
	}
	return Inst;
}

void CFascinated_Effect::Free()
{
	CParticleSystem::Free();
}


CFascinate::~CFascinate()
{
}

HRESULT CFascinate::Ready_Object(_float _size)
{
   m_pBufferCom = Add_Component<CRcShader>(L"Proto_FaciCom", L"Proto_FaciCom", ID_STATIC);
   m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
   m_pTexture = Add_Component<CTexture>(L"Proto_Fascinated", L"Proto_Fascinated", ID_STATIC);
   // m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
   m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
   m_pBufferCom->Set_TextureOption(0, 0, 0);


   m_pTransCom->Set_Scale(_size, _size, _size);
   // m_fSpeed = 6.f;
   m_fTime = 4.f;
   m_fCurTime = 0.f;
   m_pTransCom->Update_Component(0.f);
   return S_OK;
}

_int CFascinate::Update_Object(const _float& fTimeDelta)
{
   if (m_fCurTime >= m_fTime)
      return OBJ_DEAD;

   m_fCurTime += fTimeDelta;

   CGameObject::Update_Object(fTimeDelta);

   // m_pTransCom->m_vAngle.y += D3DXToRadian(40.f) * fTimeDelta * m_fSpeed;

   m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

   Add_RenderGroup(RENDER_NONALPHA, this);

   return OBJ_NOEVENT;
}

void CFascinate::Render_Object()
{
   CGameObject::Render_Object();
   m_pBufferCom->Render_Buffer();
}

void CFascinate::LateUpdate_Object()
{
   CGameObject::LateUpdate_Object();
}

void CFascinate::PreRender_Particle()
{
}

void CFascinate::PostRender_Particle()
{
}

void CFascinate::SetPos(const _vec3& vPos)
{
   m_pTransCom->m_vInfo[INFO_POS] = vPos;
}

CFascinate* CFascinate::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
   CFascinate* Inst = new CFascinate(pGraphicDev);

   if (FAILED(Inst->Ready_Object(_size)))
   {
      return nullptr;
   }
   return Inst;
}

void CFascinate::Free()
{
   CGameObject::Free();
}
#pragma endregion

#pragma endregion


#pragma region UVAnimation
#pragma region ShockPowder

CShock_Powder::CShock_Powder(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	
}

CShock_Powder::~CShock_Powder()
{
}

HRESULT CShock_Powder::Ready_Object()
{
	static _vec3 vv{ 0.f,0.f,0.f };
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_RcShaderCom", L"Proto_RcShaderCom",ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom",L"Proto_TransformCom",ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Electric", L"Proto_Electric", ID_STATIC);
	m_pBufferCom->Set_TextureOption(_uint(CGameUtilMgr::GetRandomFloat(10.f, 15.f)), 4, 4);


	m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));

	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());

	// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	// _vec3 pPos;
	// pPlayerTransform->Get_Info(INFO_POS, &pPos);
	//
	// m_pTransCom->Set_Pos(pPos.x + CGameUtilMgr::GetRandomFloat(-3.f, 3.f),
	// 	pPos.y + 0.5f, pPos.z + CGameUtilMgr::GetRandomFloat(-3.f, 3.f));


	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CShock_Powder::Update_Object(const _float& fTimeDelta)
{
	if (m_bDoOnce)
	{
		const _vec3 pPos = m_pTransCom->m_vInfo[INFO_POS];
		m_pTransCom->Set_Pos(pPos.x + CGameUtilMgr::GetRandomFloat(-3.f, 3.f),
			pPos.y + 0.5f, pPos.z + CGameUtilMgr::GetRandomFloat(-3.f, 3.f));
		m_bDoOnce = false;
	}

	if (m_fCurTime > m_fTime)
		return OBJ_DEAD;
	m_fCurTime += fTimeDelta;

	CGameObject::Update_Object(fTimeDelta);
	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	m_pTransCom->Set_Scale(CGameUtilMgr::GetRandomFloat(0.7f,2.5f), CGameUtilMgr::GetRandomFloat(0.7f, 2.5f), CGameUtilMgr::GetRandomFloat(0.7f, 2.5f));

	_float fY = CGameUtilMgr::GetRandomFloat(0.f, 2.f);
	m_pTransCom->Rotation(ROT_Y, D3DXToRadian(fY * fTimeDelta));

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CShock_Powder::Render_Object()
{
	CGameObject::Render_Object();



	m_pBufferCom->Check_Alpha(true);
	m_pBufferCom->Render_Buffer();

}

void CShock_Powder::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CShock_Powder::PreRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CShock_Powder::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CShock_Powder* CShock_Powder::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShock_Powder* Inst = new CShock_Powder(pGraphicDev);

	if (FAILED(Inst->Ready_Object()))
	{
		return nullptr;
	}
	return Inst;
}

void CShock_Powder::Free()
{
	CGameObject::Free();
}
/*-----------------
 * ChainLightning
 -----------------*/
CChainLightning::CChainLightning(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	
}

CChainLightning::~CChainLightning()
{
}

HRESULT CChainLightning::Ready_Object()
{
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_ChainLightningCom", L"Proto_ChainLightningCom",ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom",L"Proto_TransformCom",ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Electric", L"Proto_Electric", ID_STATIC);
	m_pBufferCom->Set_TextureOption(_uint(CGameUtilMgr::GetRandomFloat(10.f, 15.f)), 4, 4);
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());

	m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CChainLightning::Update_Object(const _float& fTimeDelta)
{
	if (m_fCurTime > m_fTime)
		return OBJ_DEAD;

	CGameObject::Update_Object(fTimeDelta);

	m_fCurTime += fTimeDelta;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	m_pTransCom->Set_Scale(CGameUtilMgr::GetRandomFloat(0.5f,1.5f), CGameUtilMgr::GetRandomFloat(0.5f,1.5f), CGameUtilMgr::GetRandomFloat(0.5f,1.5f));

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CChainLightning::Render_Object()
{
	m_pBufferCom->Check_Alpha(true);
	m_pBufferCom->Render_Buffer();
}

void CChainLightning::PreRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CChainLightning::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CChainLightning::SetSpark()
{
	m_bSpark = true;
	m_fOffset = CGameUtilMgr::GetRandomFloat(0.f, 1.f);
	m_pTransCom->Update_Component(0.f);
}

void CChainLightning::SetSparkPos(const _vec3& vBot, const _vec3& vTop)
{
	D3DXVec3Lerp(&m_pTransCom->m_vInfo[INFO_POS],
		&vBot,
		&vTop,
		m_fOffset);
}

CChainLightning* CChainLightning::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CChainLightning* Inst = new CChainLightning(pGraphicDev);

	if (FAILED(Inst->Ready_Object()))
	{
		return nullptr;
	}
	return Inst;
}

void CChainLightning::Free()
{
	CGameObject::Free();
}


#pragma endregion

#pragma region UVCircle
CUVCircle::~CUVCircle()
{
}

HRESULT CUVCircle::Ready_Object(_float _size, CIRCLETYPE _type)
{
	static _vec3 vv{ 0.f,0.f,0.f };
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_ShaderCircleCom", L"Proto_RcShaderCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Circle", L"Proto_Circle", ID_STATIC);
	m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));

	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());

	if (_type == SHOCK)
	{
		m_pBufferCom->Set_TextureOption(2, 4, 2);
		// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// pPlayerTransform->Get_Info(INFO_POS, &pPos);
		// m_pTransCom->Set_Pos(pPos.x, pPos.y + 0.5f, pPos.z);
		m_pTransCom->m_vInfo[INFO_POS].y += 0.5f;
		m_pTransCom->Set_Scale(_size, _size, _size);
	}
	else if (_type == FIREWORK)
	{
		m_pBufferCom->Set_TextureOption(3, 4, 2);
		_vec3 vPos = Get_GameObject<CFireWork_Fuze>(LAYER_EFFECT, L"FireWork_Fuze")->Get_FuzePos();
		m_pTransCom->Set_Pos(vPos.x, vPos.y + 0.5f, vPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
	}
	else if (_type == CREEPER)
	{
		// 크리퍼 포인터로 바꿔야함
		m_pBufferCom->Set_TextureOption(7, 4, 2);
		// m_pTransCom->Set_Pos(m_vPos.x, m_vPos.y + 0.5f, m_vPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
	}
	else if (_type == GOLEM)
	{
		//골렘 포인터로 바꿔야함 손 위치 받아오기 
		m_pBufferCom->Set_TextureOption(3, 4, 2);

		/*CTransform* pGolem= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pGolem = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}
		_vec3 pPos;
		pGolem->Get_Info(INFO_POS, &pPos);
		m_pTransCom->Set_Pos(pPos.x, pPos.y + 0.5f, pPos.z);*/
		m_pTransCom->Set_Scale(_size, _size, _size);
	}

	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CUVCircle::Update_Object(const _float& fTimeDelta)
{
	if (m_pBufferCom->Check_TextureCnt(3,1))
		return OBJ_DEAD;
	
	CGameObject::Update_Object(fTimeDelta);
	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;

}

void CUVCircle::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CUVCircle::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CUVCircle::PreRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CUVCircle::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

CUVCircle* CUVCircle::Create(LPDIRECT3DDEVICE9 pGraphicDev,_float _size, CIRCLETYPE _type)
{
	CUVCircle* Inst = new CUVCircle(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size, _type)))
	{
		return nullptr;
	}
	return Inst;
}

void CUVCircle::Free()
{
	CGameObject::Free();
}

#pragma endregion

#pragma region UV Cloud
CCloud::~CCloud() = default;

HRESULT CCloud::Ready_Object(_float _size, CLOUDTYPE _type)
{
	
	m_pTexture = Add_Component<CTexture>(L"Proto_Cloud", L"Proto_Cloud_Cloud", ID_STATIC);

	

	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	_vec3 pPos;
	pPlayerTransform->Get_Info(INFO_POS, &pPos);

	if (_type == WALK)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_WalkCloudCom", L"Proto_WalkCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		m_pBufferCom->Set_TextureOption(5, 4, 2);
		// CTransform*	pPlayer = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// pPlayer->Get_Info(INFO_POS, &pPos);
		// m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.5f;
		m_fCurTime = 0.f;
		m_fSpeed = 0.75f;

		_vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
		m_vVelocity.x = 0.f;
		m_vVelocity.z = 0.f;

	}
	else if (_type == ROLL)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_RollCloudCom", L"Proto_RollCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		m_pBufferCom->Set_TextureOption(5, 4, 2);
		// CTransform*	pPlayer = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// pPlayer->Get_Info(INFO_POS, &pPos);
		// m_pTransCom->Set_Pos(pPos.x, pPos.y + 0.5f, pPos.z);
		m_pTransCom->m_vInfo[INFO_POS].y += 0.5f;
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.5f;
		m_fCurTime = 0.f;
		m_fSpeed = 0.5f;

		_vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
	}
	else if (_type == SHOCKPOWDER)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_ShockPowderCloudCom", L"Proto_ShockPowderCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		m_pBufferCom->Set_TextureOption(20, 4, 2);
		// CTransform*	pPlayer = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// pPlayer->Get_Info(INFO_POS, &pPos);
		// m_pTransCom->Set_Pos(pPos.x, pPos.y + 0.5f, pPos.z);
		m_pTransCom->m_vInfo[INFO_POS].y += 0.5f;
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 1.8f;
		m_fCurTime = 0.f;
		m_fSpeed = 3.5f;

		_vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
		m_vVelocity.y = 0.f;
	}
	else if (_type == CREEPEREX)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_CreeperExCloudCom", L"Proto_CreeperExCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		// 크리퍼 포인터로 바꿔야함
		m_pBufferCom->Set_TextureOption(_uint(CGameUtilMgr::GetRandomFloat(10.f,15.f)), 4, 2);

		// m_pTransCom->Set_Pos(m_vPos.x, m_vPos.y + 0.5f, m_vPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = CGameUtilMgr::GetRandomFloat(0.5f, 2.f);
		m_fTime = 1.2f;
		m_fCurTime = 0.f;
		m_fSpeed = 3.5f;

		_vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
	}
	else if (_type == DECAL)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_WalkCloudCom", L"Proto_WalkCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		// m_pBufferCom->Set_TextureOption(_uint(CGameUtilMgr::GetRandomFloat(7.f, 15.f)), 4, 2);
		m_pBufferCom->Set_TextureOption(5, 4, 2);

		// CTransform*	pPlayer = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// _vec3 pLook;
		// pPlayer->Get_Info(INFO_POS, &pPos);
		// pPlayer->Get_Info(INFO_LOOK, &pLook);
		//
		// m_pTransCom->m_vInfo[INFO_POS] = pPos + pLook * 2.f;
		// m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.7f;
		m_fCurTime = 0.f;
		m_fSpeed = 3.5f;

		_vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		// m_vVelocity.x += pLook.x;
		// m_vVelocity.y += pLook.y;

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
		// m_vVelocity.x = 0.f;
		// m_vVelocity.z = 0.f;

	}
	else if (_type == MONSTERCLOUD)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_WalkCloudCom", L"Proto_WalkCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		// m_pBufferCom->Set_TextureOption(_uint(CGameUtilMgr::GetRandomFloat(7.f, 15.f)), 4, 2);
		m_pBufferCom->Set_TextureOption(5, 4, 2);

	/*	CTransform* pGolem= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pGolem = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}*/
		/*_vec3 pPos;
		_vec3 pLook;*/


		//pGolem->Get_Info(INFO_POS, &pPos);
		//pGolem->Get_Info(INFO_LOOK, &pLook);

		//m_pTransCom->m_vInfo[INFO_POS] = pPos + pLook * 2.f;
		// m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.7f;
		m_fCurTime = 0.f;
		m_fSpeed = 3.5f;

		_vec3 min = _vec3(-2.0f, 1.0f, -2.0f);
		_vec3 max = _vec3(2.0f, 1.0f, 2.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		//m_vVelocity.x += pLook.x;
		//m_vVelocity.y += pLook.y;

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
	}
	else if (_type == GOLEMWINDMILL)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_ShockPowderCloudCom", L"Proto_ShockPowderCloudCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
		m_pBufferCom->Set_TextureOption(20, 4, 2);
		/*CTransform* pGolem= nullptr;
		for (auto& e : Get_Layer(LAYER_ENEMY)->Get_MapObject())
		{
			if (CRedStoneMonstrosity* red = dynamic_cast<CRedStoneMonstrosity*>(e.second))
			{
				pGolem = red->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
			}
		}
		_vec3 pPos;
		pGolem->Get_Info(INFO_POS, &pPos);
		m_pTransCom->Set_Pos(pPos.x, pPos.y + 2.f, pPos.z);*/
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 3.f;
		m_fCurTime = 0.f;
		m_fSpeed = 3.5f;

		_vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		_vec3 max = _vec3(1.0f, 1.0f, 1.0f);

		CGameUtilMgr::GetRandomVector(
			&m_vVelocity,
			&min,
			&max);

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
		m_vVelocity.y = 0.f;
	}
	// m_pTransCom->Set_Pos(pPos.x,
	// 	pPos.y, pPos.z);
	// IM_LOG("%f, %f", m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].z);

	// _vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
	// _vec3 max = _vec3(1.0f, 1.0f, 1.0f);

	// CGameUtilMgr::GetRandomVector(
	// 	&m_vVelocity,
	// 	&min,
	// 	&max);
	D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);

	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

void CCloud::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

CCloud* CCloud::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, CLOUDTYPE _type)
{
	CCloud* Inst = new CCloud(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size, _type)))
	{
		return nullptr;
	}
	return Inst;
}

_int CCloud::Update_Object(const _float& fTimeDelta)
{
	if (m_fCurTime > m_fTime)
		return OBJ_DEAD;
	m_fCurTime += fTimeDelta;

	CGameObject::Update_Object(fTimeDelta);

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;
	
	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	// 삼각함수 적용해보기
	// m_vVelocity.y = GetRandomFloat(0.5f, 1.0f) * 1.f;
	vPos += m_vVelocity * fTimeDelta * m_fSpeed;
	
	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CCloud::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}



void CCloud::PreRender_Particle()
{
	// m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	// m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CCloud::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CCloud::Free()
{
	CGameObject::Free();
}

#pragma endregion

#pragma region Crack


CCrack::~CCrack()
{
}

HRESULT CCrack::Ready_Object(_float _size, CRACKTYPE _type)
{
	static _vec3 vv{ 0.f,0.f,0.f };


	//m_pTransCom->Set_Scale(_float(rand() % 2)  + 1, _float(rand() % 2) + 1, _float(rand() % 2 ) + 1);
	//


	if (_type == GOLEM_SPIT_CRACK)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_CrackCom", L"Proto_CrackCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Crack", L"Proto_Crack", ID_STATIC);
		m_pBufferCom->Set_TextureOption(15, 2, 2);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		_vec3 pPos;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);
		
		m_pTransCom->Set_Pos(pPos.x, pPos.y + 4.f, pPos.z);
		//IM_LOG("%f, %f", m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_iCrackType = GOLEM_SPIT_CRACK;
	}

	else if (_type == EXE_DECAL)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_Crack_DecalCom", L"Proto_Crack_DecalCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Crack", L"Proto_Crack", ID_STATIC);
		m_pBufferCom->Set_TextureOption(15, 2, 2);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		// _vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
		// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// _vec3 pLook;
		// pPlayerTransform->Get_Info(INFO_POS, &pPos);
		// pPlayerTransform->Get_Info(INFO_LOOK, &pLook);
		// vPos = pPos + pLook * 3.f;
		// vPos.y = 0.3f + Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap")->GetHeight(vPos.x, vPos.z);

		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.6f;
		m_fCurTime = 0.f;
		m_iCrackType = EXE_DECAL;
	}
	else if (_type == LAZER)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_Lazer_Com", L"Proto_Lazer_Com", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_LazerLightTex", L"Proto_LazerLightTex", ID_STATIC);
		m_pBufferCom->Set_TextureOption(15, 2, 2);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		_vec3 pPos;
		_vec3 pLook;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);
		pPlayerTransform->Get_Info(INFO_LOOK, &pLook);
		m_pTransCom->m_vInfo[INFO_POS] = pPos + pLook * 3.f;
		m_pTransCom->Set_Pos(pPos.x, pPos.y+ 2.f, pPos.z);
		 m_pTransCom->Rotation(ROT_Y, D3DXToRadian(45.f));
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.6f;
		m_fCurTime = 0.f;
		m_iCrackType = LAZER;
	}
	else if (_type == LOADINGBOX)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_LoadingBoxCom", L"Proto_LoadingBoxCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_LoadingBoxTex", L"Proto_LoadingBoxTex", ID_STATIC);
		m_pBufferCom->Set_TextureOption(10, 8, 8);
		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		_vec3 pPos;
		_vec3 pLook;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);
		m_pTransCom->Set_Pos(pPos.x, pPos.y + 4.f, pPos.z);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 30.f;
		m_fCurTime = 0.f;
		m_iCrackType = LOADINGBOX;
	}

	m_fCurTime = 0;
	m_fTime = 1.0f;
	m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));

	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CCrack::Update_Object(const _float& fTimeDelta)
{
	if (m_fCurTime > m_fTime && m_iCrackType != LOADINGBOX)
	{
		if (m_iCrackType == GOLEM_SPIT_CRACK)
		{
			CEffectFactory::Create<CSphereEffect>("Golem_Melee_M", L"Golem_Melee_M", m_pTransCom->m_vInfo[INFO_POS]);
			CEffectFactory::Create<CSphereEffect>("Golem_Melee_Shpere_M", L"Golem_Melee_Shpere_M", m_pTransCom->m_vInfo[INFO_POS]);
			Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pTransCom->m_vInfo[INFO_POS], 0.3f, D3DXCOLOR(0.8f, 0.25f, 0.1f, 1.f), 5, 0.5f);
		}
		return OBJ_DEAD;
	}

	if(m_iCrackType == LOADINGBOX)
	{
		auto tmp = m_pBufferCom->Get_WidthTextureCnt();
		auto dest = m_pBufferCom->Get_HeightTextureCnt();

		if (tmp == 2 && dest == 7)
		{
			return OBJ_DEAD;
		}
	}

	m_fCurTime += fTimeDelta;

	CGameObject::Update_Object(fTimeDelta);
	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;


	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CCrack::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CCrack::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CCrack::PreRender_Particle()
{
}

void CCrack::PostRender_Particle()
{
}

CCrack* CCrack::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, CRACKTYPE _type)
{
	CCrack* Inst = new CCrack(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size,_type)))
	{
		return nullptr;
	}
	return Inst;
}

void CCrack::Free()
{
	CGameObject::Free();
}
#pragma endregion

#pragma region GolemSpit
CGolemSpit::~CGolemSpit()
{
}

HRESULT CGolemSpit::Ready_Object(_float _size)
{
	m_pTexture = Add_Component<CTexture>(L"Proto_Spit", L"Proto_Spit", ID_STATIC);
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_SpitCom", L"Proto_SpitCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));

	

	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	_vec3 pPos;
	pPlayerTransform->Get_Info(INFO_POS, &pPos);
	m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);

	m_pTransCom->m_vInfo[INFO_POS].y += 5.f;

		m_pTransCom->Rotation(ROT_X, D3DXToRadian(45.f));
		m_pBufferCom->Set_TextureOption(0, 0, 0);
		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fTime = 0.5f;
		m_fCurTime = 0.f;
		m_fSpeed = 20.f;

	
		pPlayerTransform->Get_Info(INFO_LOOK, &m_vVelocity);
		// _vec3 min = _vec3(-1.0f, 1.0f, -1.0f);
		// _vec3 max = _vec3(1.0f, 1.0f, 1.0f);
		//
		// CGameUtilMgr::GetRandomVector(
		// 	&m_vVelocity,
		// 	&min,
		// 	&max);

		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
		// m_vVelocity.x = 0.f;
		// m_vVelocity.z = 0.f;
		
		m_fSpeed = 1.5f;
		m_pTransCom->Update_Component(0.f);
		return S_OK;
}

_int CGolemSpit::Update_Object(const _float& fTimeDelta)
{

	if (m_fCurTime > m_fTime)
	{
		CEffectFactory::Create<CSphereEffect>("Golem_Melee_M", L"Golem_Melee_M", m_pTransCom->m_vInfo[INFO_POS]);
		CEffectFactory::Create<CSphereEffect>("Golem_Spit_Sphere", L"Golem_Spit_Sphere", m_pTransCom->m_vInfo[INFO_POS]);
		CEffectFactory::Create<CUVCircle>("Creeper_Explosion", L"Creeper_Explosion", m_pTransCom->m_vInfo[INFO_POS]);
		Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pTransCom->m_vInfo[INFO_POS], 0.3f, D3DXCOLOR(0.8f,0.25f,0.1f,1.f), 5, 0.5f);
		for (int i = 0; i < 5; i++)
		{
			CEffectFactory::Create<CCloud>("Creeper_Cloud", L"Creeper_Cloud", m_pTransCom->m_vInfo[INFO_POS]);
		}
		return OBJ_DEAD;
	}
	m_fCurTime += fTimeDelta;

	CGameObject::Update_Object(fTimeDelta);

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;
	
	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	// 삼각함수 적용해보기
	m_vVelocity *= CGameUtilMgr::GetRandomFloat(0.5f, 1.0f) * 1.f;

	m_pTransCom->m_vAngle.y += D3DXToRadian(60.f) * fTimeDelta;



	// vPos.x += m_vVelocity.x * fTimeDelta * m_fSpeed;
	// vPos.y -= m_vVelocity.y * fTimeDelta * m_fSpeed ;

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CGolemSpit::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CGolemSpit::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CGolemSpit::PreRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);

}

void CGolemSpit::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);

}

CGolemSpit* CGolemSpit::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
	CGolemSpit* Inst = new CGolemSpit(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size)))
	{
		return nullptr;
	}
	return Inst;
}

void CGolemSpit::Free()
{
	CGameObject::Free();
}


#pragma endregion

#pragma region Stun
CStun::~CStun()
{
}

HRESULT CStun::Ready_Object(_float _size)
{
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_StunCom", L"Proto_StunCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Stun", L"Proto_Stun", ID_STATIC);
	m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
	m_pBufferCom->Set_TextureOption(0,0,0);

	
	m_pTransCom->Set_Scale(_size, _size, _size);
	m_fSpeed = 6.f;
	m_fTime = 2.f;
	m_fCurTime = 0.f;
	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CStun::Update_Object(const _float& fTimeDelta)
{
	if (m_fCurTime >= m_fTime)
		return OBJ_DEAD;

	m_fCurTime += fTimeDelta;

	CGameObject::Update_Object(fTimeDelta);

	m_pTransCom->m_vAngle.y += D3DXToRadian(40.f) * fTimeDelta * m_fSpeed;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CStun::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CStun::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CStun::PreRender_Particle()
{
}

void CStun::PostRender_Particle()
{
}

void CStun::SetPos(const _vec3& vPos)
{
	m_pTransCom->m_vInfo[INFO_POS] = vPos;
}

CStun* CStun::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
	CStun* Inst = new CStun(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size)))
	{
		return nullptr;
	}
	return Inst;
}

void CStun::Free()
{
	CGameObject::Free();
}

#pragma endregion 

#pragma region Lava_Particle

CLava_Particle::~CLava_Particle()
{
}

HRESULT CLava_Particle::Ready_Object(_float _size, LAVATYPE _type)
{
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_Lava_ParticleCom", L"Proto_Lava_ParticleCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Lava_Particle", L"Proto_Lava_Particle", ID_STATIC);
	// m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
	m_pBufferCom->Set_TextureOption(6, 4, 2);

	if (_type == FALLINLAVA)
	{
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f));
	}
	else if (_type == FUZEPARTICLE)
	{
		 //CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		 //_vec3 pPos;
		 //pPlayerTransform->Get_Info(INFO_POS, &pPos);
		 //m_pTransCom->Set_Pos(pPos.x, pPos.y, pPos.z);
		
		 //m_pTransCom->Set_Scale(_size, _size, _size);
		 //m_fSpeed = 3.f;
		 //m_fTime = 0.6f;
		 //m_fCurTime = 0.f;
	}
	
	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CLava_Particle::Update_Object(const _float& fTimeDelta)
{
	if (m_fCurTime > m_fTime)
		return OBJ_DEAD;

	CGameObject::Update_Object(fTimeDelta);

	m_fCurTime += fTimeDelta;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	m_pTransCom->Set_Scale(CGameUtilMgr::GetRandomFloat(1.5f, 2.5f), CGameUtilMgr::GetRandomFloat(1.5f, 2.5f), CGameUtilMgr::GetRandomFloat(1.5f, 2.5f));

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CLava_Particle::Render_Object()
{
	m_pBufferCom->Check_Alpha(true);
	m_pBufferCom->Render_Buffer();
}

void CLava_Particle::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CLava_Particle::PreRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CLava_Particle::PostRender_Particle()
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CLava_Particle::SetFlame()
{
	m_bFlame = true;
	m_fOffset = CGameUtilMgr::GetRandomFloat(0.f, 1.f);
	m_pTransCom->Update_Component(0.f);
}

void CLava_Particle::SetFlamePos(const _vec3 & vBot, const _vec3 & vTop)
{
	D3DXVec3Lerp(&m_pTransCom->m_vInfo[INFO_POS],
		&vBot,
		&vTop,
		m_fOffset);
}

CLava_Particle* CLava_Particle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, LAVATYPE _type)
{
	CLava_Particle* Inst = new CLava_Particle(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size, _type)))
	{
		return nullptr;
	}
	return Inst;
}

void CLava_Particle::Free()
{
	CGameObject::Free();
}



#pragma endregion

#pragma region HealCircle


CHealCircle::~CHealCircle()
{
}

HRESULT CHealCircle::Ready_Object(_float _size, _float _rad, HealCircleType _type)
{
	m_eType = _type;

	if (m_eType == HEAL)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_HealCom", L"Proto_HealCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Heal", L"Proto_Heal", ID_STATIC);
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(_rad));
		// m_pTransCom->Rotation(ROT_Z, D3DXToRadian(30.f));

		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		m_pBufferCom->Set_TextureOption(0, 0, 0);

	// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	// _vec3 pPos;
	// pPlayerTransform->Get_Info(INFO_POS, &pPos);
	// m_pTransCom->Set_Pos(pPos.x, pPos.y+1.5f, pPos.z);
	// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	// m_pTransCom->m_vInfo[INFO_POS].y = pPlayerTransform->m_vInfo[INFO_POS].y;

		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fSpeed = 4.f;
		m_fTime = 1.2f;
		m_fCurTime = 0.f;
	}
	else if (m_eType == BLUE_CIRCLE)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_BlueCom", L"Proto_BlueCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Heal", L"Proto_Heal", ID_STATIC);
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(_rad));
		// m_pTransCom->Rotation(ROT_Z, D3DXToRadian(30.f));

		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		m_pBufferCom->Set_TextureOption(0, 0, 0);

		// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// pPlayerTransform->Get_Info(INFO_POS, &pPos);
		// m_pTransCom->Set_Pos(pPos.x, pPos.y+1.5f, pPos.z);
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTransCom->m_vInfo[INFO_POS].y = pPlayerTransform->m_vInfo[INFO_POS].y;

		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fSpeed = 4.f;
		m_fTime = 6.1f;
		m_fCurTime = 0.f;
	}

	else if (m_eType == RED_CIRCLE)
	{
		m_pBufferCom = Add_Component<CRcShader>(L"Proto_HealCom", L"Proto_HealCom", ID_STATIC);
		m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTexture = Add_Component<CTexture>(L"Proto_Heal", L"Proto_Heal", ID_STATIC);
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(_rad));
		// m_pTransCom->Rotation(ROT_Z, D3DXToRadian(30.f));

		m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
		m_pBufferCom->Set_TextureOption(0, 0, 0);

		// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		// _vec3 pPos;
		// pPlayerTransform->Get_Info(INFO_POS, &pPos);
		// m_pTransCom->Set_Pos(pPos.x, pPos.y+1.5f, pPos.z);
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		m_pTransCom->m_vInfo[INFO_POS].y = pPlayerTransform->m_vInfo[INFO_POS].y;

		m_pTransCom->Set_Scale(_size, _size, _size);
		m_fSpeed = 4.f;
		m_fTime = 6.1f;
		m_fCurTime = 0.f;
	}
	m_pTransCom->Update_Component(0.f);

	return S_OK;
}

_int CHealCircle::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (m_fCurTime >= m_fTime)
	{
		return OBJ_DEAD;
	}

	if (m_eType == HEAL)
	{
		const _vec3& pPos = m_pFollow->m_vInfo[INFO_POS];

		m_pTransCom->m_vInfo[INFO_POS].x = pPos.x;
		m_pTransCom->m_vInfo[INFO_POS].z = pPos.z;

		m_pTransCom->m_vInfo[INFO_POS].y += fTimeDelta * 10.f;

		if (m_pTransCom->m_vInfo[INFO_POS].y >= pPos.y + 3.f)
		{
			m_pTransCom->m_vInfo[INFO_POS].y = pPos.y;
		}

	}
	else if (m_eType == BLUE_CIRCLE)
	{
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		_vec3 pPos;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);

		// m_pTransCom->m_vAngle.y += D3DXToRadian(CGameUtilMgr::GetRandomFloat(10.f,60.f)) * fTimeDelta * CGameUtilMgr::GetRandomFloat(3.f, 10.f);
		m_pTransCom->m_vAngle.z += D3DXToRadian(CGameUtilMgr::GetRandomFloat(-10.f, 90.f)) * fTimeDelta * CGameUtilMgr::GetRandomFloat(3.f, 20.f);
		m_pTransCom->m_vAngle.x += D3DXToRadian(CGameUtilMgr::GetRandomFloat(-30.f, 60.f)) * fTimeDelta * CGameUtilMgr::GetRandomFloat(3.f, 10.f);

		// m_pTransCom->m_vInfo[INFO_POS].y += fTimeDelta * 10.f;
		//
		// if (m_pTransCom->m_vInfo[INFO_POS].y >= pPos.y + 3.f)
		// {
		// 	m_pTransCom->m_vInfo[INFO_POS].y = pPos.y;
		// }
	}

	else if (m_eType == RED_CIRCLE)
	{
		CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
		_vec3 pPos;
		pPlayerTransform->Get_Info(INFO_POS, &pPos);

		// m_pTransCom->m_vAngle.y += D3DXToRadian(CGameUtilMgr::GetRandomFloat(10.f, 60.f)) * fTimeDelta * CGameUtilMgr::GetRandomFloat(3.f, 10.f);
		m_pTransCom->m_vAngle.z += D3DXToRadian(CGameUtilMgr::GetRandomFloat(-10.f, 90.f)) * fTimeDelta * CGameUtilMgr::GetRandomFloat(1.f, 20.f);
		m_pTransCom->m_vAngle.x += D3DXToRadian(CGameUtilMgr::GetRandomFloat(-30.f, 60.f)) * fTimeDelta * CGameUtilMgr::GetRandomFloat(3.f, 10.f);


		// m_pTransCom->m_vInfo[INFO_POS].y += fTimeDelta * 10.f;
		//
		// if (m_pTransCom->m_vInfo[INFO_POS].y >= pPos.y + 3.f)
		// {
		// 	m_pTransCom->m_vInfo[INFO_POS].y = pPos.y;
		// }
	}

	// m_pTransCom->m_vAngle.y += D3DXToRadian(50.f) * fTimeDelta * m_fSpeed;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	m_fCurTime += fTimeDelta;


	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CHealCircle::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CHealCircle::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CHealCircle::PreRender_Particle()
{
}

void CHealCircle::PostRender_Particle()
{
}

CHealCircle* CHealCircle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, _float _rad, HealCircleType _type)
{
	CHealCircle* Inst = new CHealCircle(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size, _rad,_type)))
	{
		return nullptr;
	}
	return Inst;
}

void CHealCircle::Free()
{
	Safe_Release(m_pFollow);
	CGameObject::Free();
}

#pragma endregion 

#pragma region HeartParticle

CHeartParticle::~CHeartParticle()
{
}

HRESULT CHeartParticle::Ready_Object(_float _size)
{
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_HeartParticleCom", L"Proto_HeartParticleCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Heal_Particle", L"Proto_Heal_Particle", ID_STATIC);
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
	m_pBufferCom->Set_TextureOption(_uint(CGameUtilMgr::GetRandomFloat(15.f,20.f)), 4, 1);

	m_pTransCom->Rotation(ROT_Y, D3DXToRadian(45.f));
	// pPlayerTransform->Get_Info(INFO_POS, &m_vOrigin);
	tmp = CGameUtilMgr::GetRandomFloat(-1.5f, 1.5f);
	desk = CGameUtilMgr::GetRandomFloat(-1.5f, 1.5f);
	m_pTransCom->Set_Scale(_size, _size, _size);
	m_fSpeed = CGameUtilMgr::GetRandomFloat(3.f, 7.f);
	m_fTime = 1.5f;
	m_fCurTime = 0.f;

	// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	// m_pTransCom->m_vInfo[INFO_POS].y = pPlayerTransform->m_vInfo[INFO_POS].y;

	m_pTransCom->Update_Component(0.f);
	return S_OK;
}

_int CHeartParticle::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (m_fCurTime >= m_fTime)
		return OBJ_DEAD;


	m_fCurTime += fTimeDelta;

	if(_int(fTimeDelta)% 2 == 0)
		m_pTransCom->m_vScale *= fTimeDelta * 58.5f;

	m_pTransCom->m_vAngle.y += D3DXToRadian(CGameUtilMgr::GetRandomFloat(-40.f, 60.f)) * fTimeDelta * m_fSpeed;

	_vec3 pPos;
	m_pFollow->Get_Info(INFO_POS, &pPos);

	m_pTransCom->m_vInfo[INFO_POS].x = pPos.x +tmp;
	m_pTransCom->m_vInfo[INFO_POS].z = pPos.z + desk;

	m_pTransCom->m_vInfo[INFO_POS].y += fTimeDelta * m_fSpeed;


	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CHeartParticle::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CHeartParticle::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CHeartParticle::PreRender_Particle()
{
}

void CHeartParticle::PostRender_Particle()
{
}

CHeartParticle* CHeartParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
	CHeartParticle* Inst = new CHeartParticle(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size)))
	{
		return nullptr;
	}
	return Inst;
}

void CHeartParticle::Free()
{
	Safe_Release(m_pFollow);
	CGameObject::Free();
}
#pragma endregion


#pragma region Lazer

CLazer::~CLazer()
{
}

HRESULT CLazer::Ready_Object(_float _size)
{
	m_pBufferCom = Add_Component<CTerrainShader>(L"Proto_LazerBeamCom", L"Proto_LazerBeamCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_LazerBeamTex", L"Proto_LazerBeamTex", ID_STATIC);
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
	m_pBufferCom->Set_UVOption( 1, 0.f, CGameUtilMgr::GetRandomFloat(0.f,0.f), 0.f, 4.f);

	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	_vec3 pPos;
	_vec3 pLook;
	pPlayerTransform->Get_Info(INFO_POS, &pPos);
	pPlayerTransform->Get_Info(INFO_LOOK, &pLook);
	pPos.y += 3.f;
	m_pTransCom->m_vInfo[INFO_POS] = pPos +pLook *2;

	m_vVelocity = pLook - pPos;
	m_pTransCom->Set_Scale(50.f, _size, _size);
	
	m_pTransCom->m_vAngle.y = pPlayerTransform->m_vAngle.y;
	m_pTransCom->Rotation(ROT_Y, D3DXToRadian(-90.f));


	_vec3 vRight = { 0.f, 0.f, 1.f };
	D3DXVec3Normalize(&pLook, &pLook);
	_float fAngle = acosf(D3DXVec3Dot(&pLook, &vRight));
	fAngle *=  180.f / PI;


	if (fAngle > 90.f)
	{
		D3DXVec3Cross(&vRight, &CGameUtilMgr::s_vUp, &pLook);
		D3DXVec3Normalize(&vRight, &vRight);
		m_pTransCom->m_vInfo[INFO_POS] += vRight * 1.f;
	}

	
	

	return S_OK;
}

_int CLazer::Update_Object(const _float& fTimeDelta)
{


	if (m_bDead)
		return OBJ_DEAD;

	CGameObject::Update_Object(fTimeDelta);
	/*CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	_vec3 pPos;
	pPlayerTransform->Get_Info(INFO_POS, &pPos);*/

	// m_pTransCom->m_vInfo[INFO_POS].y += pPos.y + fTimeDelta * 60.f;

	// m_pTransCom->m_vInfo[INFO_POS].x = pPos.x;
	// m_pTransCom->m_vInfo[INFO_POS].z = pPos.z - 0.7f;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CLazer::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CLazer::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CLazer::PreRender_Particle()
{
}

void CLazer::PostRender_Particle()
{
}

CLazer* CLazer::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
	CLazer* Inst = new CLazer(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size)))
	{
		return nullptr;
	}
	return Inst;
}

void CLazer::Free()
{
	CGameObject::Free();
}
#pragma endregion

#pragma region Gradation Beam
CGradation_Beam::~CGradation_Beam()
{
}

HRESULT CGradation_Beam::Ready_Object(_float _size)
{
	m_pBufferCom = Add_Component<CTerrainShader>(L"Proto_GradationCom", L"Proto_GradationCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_GradationTex", L"Proto_GradationTex", ID_STATIC);
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());

	// m_pBufferCom->Set_UVOption(0, CGameUtilMgr::GetRandomFloat(1.5f, 3.f), 0.5f, 1.5f, 0.f);
	m_pBufferCom->Set_UVOption(0, 0.f, 0.5f, 1.5f, 0.f);

	//CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	//_vec3 pPos;
	//_vec3 pLook;
	//pPlayerTransform->Get_Info(INFO_POS, &pPos);
	//pPlayerTransform->Get_Info(INFO_LOOK, &pLook);
	//m_pTransCom->m_vInfo[INFO_POS] = pPos + pLook * 3;
	//m_pTransCom->m_vInfo[INFO_POS].y = pPos.y -1.f;

	m_pTransCom->Set_Scale(_size, _size, 8.f);
	m_pTransCom->Rotation(ROT_Y, D3DXToRadian(90.f));
	m_pTransCom->Rotation(ROT_Z, D3DXToRadian(90.f));

	m_fTime = 5.f;
	m_fCurTime = 0.f;

	return S_OK;
}

_int CGradation_Beam::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (m_fCurTime >= m_fTime)
		return OBJ_DEAD;

	m_fCurTime += fTimeDelta;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CGradation_Beam::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();
}

void CGradation_Beam::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CGradation_Beam::PreRender_Particle()
{
}

void CGradation_Beam::PostRender_Particle()
{
}

CGradation_Beam* CGradation_Beam::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
	CGradation_Beam* Inst = new CGradation_Beam(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size)))
	{
		return nullptr;
	}
	return Inst;
}

void CGradation_Beam::Free()
{
	CGameObject::Free();
}



#pragma endregion

#pragma region Lazer Circle
CLazer_Circle::~CLazer_Circle()
{
}

HRESULT CLazer_Circle::Ready_Object(_float _size)
{
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_LazerCircleCom", L"Proto_LazerCircleCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_LazerCircleTex", L"Proto_LazerCircleTex", ID_STATIC);
	
	// m_pTransCom->Rotation(ROT_Z, D3DXToRadian(30.f));

	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture());
	m_pBufferCom->Set_TextureOption(0, 0, 0);

	// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	// _vec3 pPos;
	// pPlayerTransform->Get_Info(INFO_POS, &pPos);
	// m_pTransCom->Set_Pos(pPos.x, pPos.y+1.5f, pPos.z);
	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	_vec3 pPos;
	_vec3 pLook;
	pPlayerTransform->Get_Info(INFO_POS, &pPos);
	pPlayerTransform->Get_Info(INFO_LOOK, &pLook);
	m_pTransCom->m_vInfo[INFO_POS] = pPos + pLook * 3;

	m_vVelocity = pPos-pPos + pLook * CGameUtilMgr::GetRandomFloat(1.5f,3.f);
	m_pTransCom->Set_Scale(24.f, _size, _size);
	m_pTransCom->m_vInfo[INFO_POS].y = m_pTransCom->m_vInfo[INFO_POS].y + 1.f;
	m_pTransCom->m_vInfo[INFO_POS].x = pPos.x + pLook.x * 3 + CGameUtilMgr::GetRandomFloat(-0.5f,0.5f);
	m_pTransCom->m_vInfo[INFO_POS].z = pPos.z + pLook.z * 3 + CGameUtilMgr::GetRandomFloat(-0.5f, 0.5f);

	m_pTransCom->m_vAngle.y = pPlayerTransform->m_vAngle.y;

	m_pTransCom->Set_Scale(_size, _size, _size);
	m_fSpeed = 2.f;
	m_fTime = 0.1f;
	m_fCurTime = 0.f;
	m_vVelocity.y = 0.f;

	D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
	m_pTransCom->Update_Component(0.f);

	return S_OK;
}

_int CLazer_Circle::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (m_fCurTime >= m_fTime)
	{
		return OBJ_DEAD;
	}
	m_pTransCom->m_vInfo[INFO_POS] += m_vVelocity * fTimeDelta * 20.f;

	CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	_vec3 pPos;
	_vec3 pLook;
	pPlayerTransform->Get_Info(INFO_POS, &pPos);
	pPlayerTransform->Get_Info(INFO_LOOK, &pLook);

	// if(m_pTransCom->m_vInfo[INFO_POS] >= pPos+pLook * 10)
	// {
	// 	m_pTransCom->m_vInfo[INFO_POS] = pPos + pLook * 3;
	// }
	// Set_Pos(pPos.x, pPos.y + 1.5f, pPos.z);

	// m_pTransCom->m_vInfo[INFO_POS].y += fTimeDelta * 10.f;

	// m_pTransCom->m_vScale *= 61.5f * fTimeDelta;

	// if (m_pTransCom->m_vInfo[INFO_POS].y >= pPos.y + 3.f)
	// {
		// m_pTransCom->m_vInfo[INFO_POS].y = pPos.y;
	// }
	m_pTransCom->m_vScale *= 61.5f * fTimeDelta;

	m_fCurTime += fTimeDelta;


	// m_pTransCom->Rotation(ROT_X, D3DXToRadian(fTimeDelta*m_fSpeed));
	// m_pTransCom->m_vAngle.x += D3DXToRadian(50.f) * fTimeDelta * m_fSpeed;

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CLazer_Circle::Render_Object()
{
	CGameObject::Render_Object();
	m_pBufferCom->Render_Buffer();

}

void CLazer_Circle::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CLazer_Circle::PreRender_Particle()
{
}

void CLazer_Circle::PostRender_Particle()
{
}

CLazer_Circle* CLazer_Circle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size)
{
	CLazer_Circle* Inst = new CLazer_Circle(pGraphicDev);

	if (FAILED(Inst->Ready_Object(_size)))
	{
		return nullptr;
	}
	return Inst;
}

void CLazer_Circle::Free()
{
	CGameObject::Free();
}
#pragma endregion

#pragma endregion



