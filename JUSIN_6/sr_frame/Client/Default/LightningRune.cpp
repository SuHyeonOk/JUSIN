#include "stdafx.h"
#include "LightningRune.h"
#include "Monster.h"
#include "Weapon.h"
#include "Player.h"
#include "StatComponent.h"
#include <random>

#include "Inventory.h"

CLightningRune::CLightningRune(LPDIRECT3DDEVICE9 pGraphicDev): CRune(pGraphicDev)
{
	m_eTargetType = WEAPON_SWORD;
	m_iUItexNum = 17;
	m_eItemType = IT_RUNE;
	m_strFactoryTag = "LightningRune";

	ZeroMemory(&m_LightningLight, sizeof(D3DLIGHT9));
}

CLightningRune::~CLightningRune()
{
}

HRESULT CLightningRune::Ready_Object()
{
	CRune::Ready_Object();
	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"LightningRuneUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	m_LightningLight.Type = D3DLIGHT_POINT;
	m_LightningLight.Diffuse.r = 0.f;
	m_LightningLight.Diffuse.g = 0.f;
	m_LightningLight.Diffuse.b = 1.f;
	m_LightningLight.Ambient = {0.f, 0.f, 1.f};
	m_LightningLight.Specular = {0.f, 0.f, 1.f};
	m_LightningLight.Attenuation0 = 0.4f;
	m_LightningLight.Range = 4.f;

	m_fLightTime = 0.2f;
	m_fCurLightTime = 0.2f;

	return S_OK;
}

_int CLightningRune::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	CRune::Update_Object(fTimeDelta);
	if (m_pEquippedWeapon && m_pEquippedWeapon->IsEquipped() == false)
	{
		if (m_vecSparks.empty() == false)
		{
			for(auto& e : m_vecSparks)
				Safe_Release(e);
			m_vecSparks.clear();
		}

		return OBJ_NOEVENT;
	}

	if (m_bSpark)
	{
		if (m_fCurSparkTime < m_fSparkTime)
			m_fCurSparkTime += fTimeDelta;

		if (m_fCurSparkTime >= m_fSparkTime && m_WeaponTop && m_WeaponBot)
		{
			auto pSpark = CEffectFactory::Create<CChainLightning>("ChainLightning", L"ChainLightning");
			pSpark->AddRef();
			pSpark->SetSpark();
			
			m_vecSparks.push_back(pSpark);

			m_fCurSparkTime = 0.f;
		}

		const _matrix matTop = m_WeaponTop->GetWorldMat();
		const _matrix matBot = m_WeaponBot->GetWorldMat();
		const _vec3 vBot{matBot._41, matBot._42, matBot._43};
		const _vec3 vTop{matTop._41, matTop._42, matTop._43};

		for (auto itr = m_vecSparks.begin(); itr != m_vecSparks.end();)
		{
			auto pSpark = (*itr);
			if (pSpark->GetRef() == 0)
			{
				Safe_Release(pSpark);
				itr = m_vecSparks.erase(itr);
			}
			else
			{
				pSpark->SetSparkPos(vBot, vTop);
				++itr;
			}
		}
	}


	if (m_fLightTime < m_fCurLightTime)
	{
		m_pGraphicDev->LightEnable(1, FALSE);
	}
	else
	{
		m_fCurLightTime += fTimeDelta;
	}




	return OBJ_NOEVENT;
}

template<class BidiIter>
BidiIter random_unique(BidiIter begin, BidiIter end, size_t num_random) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        BidiIter r = begin;
        std::advance(r, rand()%left);
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}

void CLightningRune::Collision()
{
	CRune::Collision();
	const _vec3& vOwnerPos = m_pOwner->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
	CSoundMgr::GetInstance()->PlaySoundRandom({L"sfx_env_thunderPassing-001_soundWave.ogg", L"sfx_env_thunderPassing-002_soundWave.ogg", L"sfx_env_thunderPassing-003_soundWave.ogg"}, 
		vOwnerPos);

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);

	set<CGameObject*> objSet;
	Engine::GetOverlappedObject(OUT objSet, vPos, 4.f);

	vector<CGameObject*> tmp{objSet.begin(), objSet.end()};
	if (tmp.size() > 5)
		random_unique(tmp.begin(), tmp.end(), 5);

	for (auto& obj : objSet)
	{
		if (CMonster* monster = dynamic_cast<CMonster*>(obj))
		{
			monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(10, vPos, m_pOwner);
			const _vec3 vMonsterPos = monster->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS] + _vec3{0.f, 1.5f, 0.f};
			CEffectFactory::Create<CChainLightning>("ChainLightning", L"ChainLightning", vMonsterPos);
		}
	}
	//
	m_fCurLightTime = 0.f;
	m_LightningLight.Position = vOwnerPos + _vec3{0.f, 2.f, 0.f};
	m_pGraphicDev->SetLight(1, &m_LightningLight);
	m_pGraphicDev->LightEnable(1, TRUE);
}

CLightningRune* CLightningRune::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLightningRune* pInstance = new CLightningRune(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLightningRune::EquipRune(CWeapon* pWeapon)
{
	m_bSpark = true;
	m_WeaponTop = m_pOwner->Get_SkeletalPart("weapon_top");
	m_WeaponBot = m_pOwner->Get_SkeletalPart("weapon_bot");
	m_pEquippedWeapon = pWeapon;
}

void CLightningRune::UnEquipRune(CWeapon* pWeapon)
{
	for(auto& e : m_vecSparks)
		Safe_Release(e);
	m_vecSparks.clear();
	m_bSpark = false;
	m_WeaponTop = nullptr;
	m_WeaponBot = nullptr;
	m_pEquippedWeapon = nullptr;
}

void CLightningRune::Free()
{
	for(auto& e : m_vecSparks)
		Safe_Release(e);
	m_vecSparks.clear();
	CRune::Free();
}
