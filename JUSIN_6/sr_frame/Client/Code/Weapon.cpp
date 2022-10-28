#include "stdafx.h"
#include "..\Header\Weapon.h"
#include "Rune.h"
#include "Inventory.h"
CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphicDev): CEquipItem(pGraphicDev)
, m_pInventory(nullptr) , m_pRune(nullptr)
{
}

CWeapon::~CWeapon()
{
}

HRESULT CWeapon::Ready_Object()
{
	// m_pInventory = Get_GameObject<CInventory>(LAYER_GAMEOBJ, L"Inventory");
	return S_OK;
}

_int CWeapon::Update_Object(const _float & fTimeDelta)
{
	CEquipItem::Update_Object(fTimeDelta);

	

	return OBJ_NOEVENT;
}

bool CWeapon::SetRune(CRune* pRune)
{
	if (pRune == nullptr) // 룬 해제
	{
		if (m_pRune)
			m_pRune->UnEquipRune(this);
		//Safe_Release(m_pRune);
		m_pRune = nullptr;
		return true;
	}

	// 룬 장착

	if (pRune->GetTargetTypeMask() & m_eType) // 장착 가능한지 확인
	{
		// 장착 가능
		if (m_pRune) // 기존 룬이 이미 있다면 해제
		{
			m_pRune->UnEquipRune(this);
			//Safe_Release(m_pRune);
			m_pRune = nullptr;
		}

		// 그다음 장착
		m_pRune = pRune;
		m_pRune->EquipRune(this);
		//m_pRune->AddRef();
		return true;
	}

	// 장착 불가
	return false;
}

void CWeapon::Free()
{
	CEquipItem::Free();
}
