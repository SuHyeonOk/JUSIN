#include "stdafx.h"
#include "..\Header\LaserShotRune.h"
#include "Player.h"
#include "Monster.h"
#include "StatComponent.h"
CLaserShotRune::CLaserShotRune(LPDIRECT3DDEVICE9 pGraphicDev): CRune(pGraphicDev)
{
	m_eTargetType = WEAPON_CROSSBOW;
	m_iUItexNum = 15;
	m_eItemType = IT_RUNE;
	m_strFactoryTag = "LaserShotRune";
}

CLaserShotRune::~CLaserShotRune()
{
}

HRESULT CLaserShotRune::Ready_Object()
{
	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"LaserRuneUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return CRune::Ready_Object();
}

_int CLaserShotRune::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	if (!m_bUse)
		return OBJ_NOEVENT;

	CEffectFactory::Create<CLazer>("Lazer_Beam", L"Lazer_Beam");
	for (int i = 0; i < 12; i++)
	{
		CEffectFactory::Create<CLazer_Circle>("Lazer_Beam_Circle", L"Lazer_Beam_Circle");
	}

	m_bUse = false;

	return CRune::Update_Object(fTimeDelta);
}

void CLaserShotRune::LateUpdate_Object()
{
	CRune::LateUpdate_Object();
}

void CLaserShotRune::Render_Object()
{
	CRune::Render_Object();
	
}

void CLaserShotRune::Free()
{
	CRune::Free();
}

void CLaserShotRune::Collision()
{
	set<CGameObject*> objSet;

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);
	_vec3 vLook = pPlayer->GetInfo(INFO_LOOK);


	if (m_fLaserTime < m_fCurLaserTime)
	{
		m_fCurLaserTime = 0.f;
		for (_int i = 0; i < 10; ++i)
		{
			_vec3 vAttackPos = vPos + vLook * 2.5f * (_float)i;
			Engine::GetOverlappedObject(OUT objSet, vAttackPos, 0.5f);
			for (auto& obj : objSet)
			{
				if (CMonster* monster = dynamic_cast<CMonster*>(obj))
				{
					DamageType eDT = DT_STIFFEN;
					if (monster->CheckCC()) eDT = DT_END;
					monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
						->TakeDamage(8, vPos, this, eDT);
				}
			}
		}
	}
	else
		m_fCurLaserTime += CGameUtilMgr::s_fTimeDelta;
}

void CLaserShotRune::Use()
{
	m_pOwner->SpawnArrow(8, PlayerArrowType::LASER);
}


CLaserShotRune* CLaserShotRune::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLaserShotRune* pInstance = new CLaserShotRune(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
