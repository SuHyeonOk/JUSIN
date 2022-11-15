#include "stdafx.h"
#include "..\Header\RemoteInventory.h"

#include "Player.h"
#include "Rune.h"
#include "Weapon.h"


CRemoteInventory::CRemoteInventory(LPDIRECT3DDEVICE9 pGraphicDev) :CInventory(pGraphicDev)
{
}

HRESULT CRemoteInventory::Ready_Object()
{
	m_bRemote = true;
	m_iArrow = 9999;
	return S_OK;
}

_int CRemoteInventory::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (m_equipStateList.empty() == false)
	{
		list<Protocol::EquipState> equipStateList;
		{
			WRITE_LOCK;
			equipStateList.assign(m_equipStateList.begin(), m_equipStateList.end());
			m_equipStateList.clear();
		}

		for (auto& state : equipStateList)
		{
			CEquipItem* pDelete = nullptr;
			switch (state.type())
			{
				case Protocol::MELEE:
					{
						CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_pMelee);
						CRune* pRune = pWeapon->GetRune();
						if (pRune)
						{
							pRune->SetDelete();
							// Safe_Release(pRune);
						}
						pWeapon->SetDelete();
						// Safe_Release(m_pMelee);
						m_pMelee = GetEquipFromProtocol(state);
						m_pOwner->WeaponChange(IT_MELEE);
						dynamic_cast<CWeapon*>(m_pMelee)->SetInventory(this);

					}
					break;
				case Protocol::RANGE:
					{
						CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_pRange);
						CRune* pRune = pWeapon->GetRune();
						if (pRune)
						{
							pRune->SetDelete();
							// Safe_Release(pRune);
						}
						pWeapon->SetDelete();
						// Safe_Release(m_pRange);
						m_pRange = GetEquipFromProtocol(state);
						dynamic_cast<CWeapon*>(m_pRange)->SetInventory(this);
						m_pOwner->WeaponChange(IT_RANGE);
					}
					break;
				case Protocol::LEGACY:
					{
						LEGACY_SLOT eSlot = LEGACY_SLOT_END;
						if (state.legacyslot() == 1) eSlot = LEGACY_SLOT1;
						else if (state.legacyslot() == 2) eSlot = LEGACY_SLOT2;
						else if (state.legacyslot() == 3) eSlot = LEGACY_SLOT3;
						pDelete = m_arrLegacy[eSlot];
						if (pDelete)
						{
							pDelete->SetDelete();
							// Safe_Release(pDelete);
						}

						m_arrLegacy[eSlot] = GetEquipFromProtocol(state);
					}
					break;
				case Protocol::EquipType_INT_MIN_SENTINEL_DO_NOT_USE_: 
				case Protocol::EquipType_INT_MAX_SENTINEL_DO_NOT_USE_: 
				default:
					_CRASH("wrong state type");
			}

		}
		ResetWeaponEquipped();
	}

	return S_OK;
}

CRemoteInventory* CRemoteInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRemoteInventory* pInstance = new CRemoteInventory(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}
