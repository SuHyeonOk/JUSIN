#include "stdafx.h"
#include "..\Header\StormRune.h"
#include "Player.h"
#include "Monster.h"
#include "StatComponent.h"
#include <random>
#include "Inventory.h"

CStormRune::CStormRune(LPDIRECT3DDEVICE9 pGraphicDev): CRune(pGraphicDev)
{
	m_eTargetType = WEAPON_GLAIVE;
	m_iUItexNum = 18;
	m_eItemType = IT_RUNE;
	m_strFactoryTag = "StormRune";
}

CStormRune::~CStormRune()
{
}

HRESULT CStormRune::Ready_Object()
{
	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"StormRuneUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return CRune::Ready_Object();
}

_int CStormRune::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	CRune::Update_Object(fTimeDelta);
	if (m_pEquippedWeapon && m_pEquippedWeapon->IsEquipped() == false)
	{
		if (m_vecFlame.empty() == false)
		{
			for (auto& e : m_vecFlame)
				Safe_Release(e);
			m_vecFlame.clear();
		}

		return OBJ_NOEVENT;
	}

	if (m_bFlame)
	{
		if (m_fCurFlameTime < m_fFlameTime)
			m_fCurFlameTime += fTimeDelta;

		if (m_fCurFlameTime >= m_fFlameTime && m_WeaponTop && m_WeaponBot)
		{
			auto pFlame = CEffectFactory::Create<CLava_Particle>("Lava_Particle", L"Lava_Particle");

			pFlame->AddRef();
			pFlame->SetFlame();

			m_vecFlame.push_back(pFlame);

			m_fCurFlameTime = 0.f;
		}

		const _matrix matTop = m_WeaponTop->GetWorldMat();
		const _matrix matBot = m_WeaponBot->GetWorldMat();

		
		_vec3 vBot{ matBot._41, matBot._42, matBot._43 };
		_vec3 vTop{ matTop._41, matTop._42, matTop._43 };
		_vec3 vToTop = vTop - vBot;
		D3DXVec3Normalize(&vToTop, &vToTop);
		vTop += vToTop * 1.5f ;
		vBot += vToTop;

		for (auto itr = m_vecFlame.begin(); itr != m_vecFlame.end();)
		{
			auto pFlame = (*itr);
			if (pFlame->GetRef() == 0)
			{
				Safe_Release(pFlame);
				itr = m_vecFlame.erase(itr);
			}
			else
			{
				pFlame->SetFlamePos(vBot, vTop);
				++itr;
			}
		}
	}


	return OBJ_NOEVENT;
}

template<class BidiIter>
BidiIter random_unique(BidiIter begin, BidiIter end, size_t num_random) {
	size_t left = std::distance(begin, end);
	while (num_random--) {
		BidiIter r = begin;
		std::advance(r, rand() % left);
		std::swap(*begin, *r);
		++begin;
		--left;
	}
	return begin;
}



void CStormRune::LateUpdate_Object()
{
	CRune::LateUpdate_Object();
}

void CStormRune::Render_Object()
{
	CRune::Render_Object();
}

void CStormRune::Free()
{
	// m_pItemUI->SetDelete();
	for (auto& e : m_vecFlame)
		Safe_Release(e);
	m_vecFlame.clear();
	CRune::Free();
	CRune::Free();
}

void CStormRune::Collision()
{
	CRune::Collision();
	const _vec3& vOwnerPos = m_pOwner->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	CSoundMgr::GetInstance()->PlaySoundRandom({ L"sfx_env_thunderPassing-001_soundWave.ogg", L"sfx_env_thunderPassing-002_soundWave.ogg", L"sfx_env_thunderPassing-003_soundWave.ogg" },
		vOwnerPos);

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);

	set<CGameObject*> objSet;
	Engine::GetOverlappedObject(OUT objSet, vPos, 4.f);

	vector<CGameObject*> tmp{ objSet.begin(), objSet.end() };
	if (tmp.size() > 5)
		random_unique(tmp.begin(), tmp.end(), 5);

	for (auto& obj : objSet)
	{
		if (CMonster* monster = dynamic_cast<CMonster*>(obj))
		{
			monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(10, vPos, m_pOwner);
			const _vec3 vMonsterPos = monster->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS] + _vec3{ 0.f, 1.5f, 0.f };
			CEffectFactory::Create<CLava_Particle>("Lava_Particle", L"Lava_Particle", vMonsterPos);
		}
	}
}


CStormRune* CStormRune::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStormRune* pInstance = new CStormRune(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CStormRune::EquipRune(CWeapon * pWeapon)
{
	m_bFlame = true;
	m_WeaponTop = m_pOwner->Get_SkeletalPart("weapon_top");
	m_WeaponBot = m_pOwner->Get_SkeletalPart("weapon_bot");
	m_pEquippedWeapon = pWeapon;
}

void CStormRune::UnEquipRune(CWeapon * pWeapon)
{
	for (auto& e : m_vecFlame)
		Safe_Release(e);
	m_vecFlame.clear();
	m_bFlame = false;
	m_WeaponTop = nullptr;
	m_WeaponBot = nullptr;
	m_pEquippedWeapon = nullptr;
}
