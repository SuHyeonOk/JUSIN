#include "stdafx.h"
#include "SatonCircle.h"

#include "AbstFactory.h"
#include "Particle.h"
#include "Player.h"
#include "StatComponent.h"

CSatonCircle::CSatonCircle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev)
{
}

CSatonCircle::CSatonCircle(const CSatonCircle& rhs) : CGameObject(rhs)
{
}

CSatonCircle::~CSatonCircle()
{
}

HRESULT CSatonCircle::Ready_Object()
{
	CGameObject::Ready_Object();


	m_pTransform = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_fLifeTime = 6.f;
	m_fSpeed = 7.f;
	
	return S_OK;
}

_int CSatonCircle::Update_Object(const _float& fTimeDelta)
{
	if (m_bDead) return OBJ_DEAD;

	if (m_fLifeTime <= 0.f)
		m_bDead = true;

	CGameObject::Update_Object(fTimeDelta);

	if (m_bSetY == false)
	{
		_vec3& vPos = m_pTransform->m_vInfo[INFO_POS];

		m_pTransform->m_vScale = { 2.f, 2.f, 2.f };
		m_pTransform->Update_Component(0);

		m_bSetY = true;
	}

	if(m_dwMakeCircleCoolTime + 1200 < GetTickCount() && m_bIsRed)
	{
		CEffectFactory::Create<CHealCircle>("Heal_Circle_R", L"Heal_Circle_R");
		Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(m_pTransform->m_vInfo[INFO_POS], 1.f, D3DXCOLOR(1.f, 0.2f, 0.f, 0.f), 24, 0.3f, 1);

		m_dwMakeCircleCoolTime = GetTickCount();
	}
	else if(m_dwMakeCircleCoolTime + 1200 < GetTickCount() && !m_bIsRed)
	{
		CEffectFactory::Create<CHealCircle>("Blue_Circle", L"Blue_Circle");
		Get_GameObject<CMoonParticle>(LAYER_EFFECT, L"MoonParticle")->Add_Particle(m_pTransform->m_vInfo[INFO_POS], 1.f, D3DXCOLOR(0.f, 0.2f, 1.f, 0.f), 24, 0.3f, 1);

		m_dwMakeCircleCoolTime = GetTickCount();
	}

	// 수명 줄이기 코드
	m_fLifeTime -= fTimeDelta;

	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CSatonCircle::LateUpdate_Object()
{
		// shake
	// Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")	->PlayShake(0.15f, 0.4f);

	_vec3& vPos = m_pTransform->m_vInfo[INFO_POS];

	set<CGameObject*> setObj;
	Engine::GetOverlappedObject(setObj, vPos, 2.f);

	for (auto& obj : setObj)
	{
		if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
		{
			pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, vPos, this, DT_KOUKU_SYMBOL_BLUE);
			IM_LOG("SatonSymbol");
			break;
		}
	}


	DEBUG_SPHERE(vPos, 2.f, 1.f);
	m_bDead = true;
}

void CSatonCircle::Render_Object()
{
	CGameObject::Render_Object();
}

CSatonCircle* CSatonCircle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _IsRed)
{
	CSatonCircle* pInst = new CSatonCircle(pGraphicDev);
	pInst->m_bIsRed = _IsRed;

	if (FAILED(pInst->Ready_Object()))
	{
		MSG_BOX("Fail to Create Saton Circle");
		return nullptr;
	}
	return pInst;
}
