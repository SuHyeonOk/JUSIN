#include "stdafx.h"
#include "..\Header\ShockPowder.h"
#include "AbstFactory.h"
#include "Particle.h"
#include "Player.h"
#include "StatComponent.h"
#include "Monster.h"

CShockPowder::CShockPowder(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEquipItem(pGraphicDev)
{
	m_strFactoryTag = "ShockPowder";
}


CShockPowder::~CShockPowder()
{
}

HRESULT CShockPowder::Ready_Object()
{
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_iUItexNum = 14;
	m_fCurCoolTime = 10.f;
	m_fCoolTime = 10.f;

	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"ShockPowderUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);
	return S_OK;
}

_int CShockPowder::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	if (m_fCoolTime > m_fCurCoolTime)
	{
		m_fCurCoolTime += fTimeDelta;
		m_bUse = false;
		return OBJ_NOEVENT;
	}

	if (!m_bUse)
		return 0;

	m_fCurCoolTime = 0.f;

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS) + _vec3{0.f, 0.5f, 0.f};

	for (int j = 0; j < 10; j++)
	{
		CEffectFactory::Create<CShock_Powder>("Shock_Powder", L"UV_Shock_Powder", vPos);
		CEffectFactory::Create<CCloud>("ShockPowder_Cloud", L"ShockPowder_Cloud", vPos);
	}

	CEffectFactory::Create<CUVCircle>("Shock_Circle", L"Shock_Circle", vPos);

	CSoundMgr::GetInstance()->PlaySound(L"sfx_item_shockpowder-001.ogg", vPos);
	m_bUse = false;
	m_bColl = true;
	CEquipItem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CShockPowder::LateUpdate_Object()
{
	CEquipItem::LateUpdate_Object();
	if (!m_bColl)
		return;

	set<CGameObject*> objSet;

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);

	Engine::GetOverlappedObject(OUT objSet, vPos, 3.5f);
	for (auto& obj : objSet)
	{
		if (CMonster* monster = dynamic_cast<CMonster*>(obj))
		{
			DamageType eDT = DT_END;
			if (m_iAttackCnt == 0) eDT = DT_STUN;
			if (monster->CheckCC()) eDT = DT_END;
			monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, vPos, this, eDT);
		}
	}

	DEBUG_SPHERE(vPos, 3.5f, 1.f);
	m_bColl = false;
	
}

void CShockPowder::Render_Object()
{
}

void CShockPowder::Free()
{
	CEquipItem::Free();
}

CShockPowder * CShockPowder::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShockPowder* pInstance = new CShockPowder(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShockPowder::Equipment(SkeletalPart * pSkeletalPart)
{
}


