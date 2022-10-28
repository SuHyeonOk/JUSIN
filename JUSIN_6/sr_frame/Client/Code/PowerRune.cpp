#include "stdafx.h"
#include "..\Header\PowerRune.h"

CPowerRune::CPowerRune(LPDIRECT3DDEVICE9 pGraphicDev) : CRune(pGraphicDev)
{
	m_eTargetType = WEAPON_SWORD | WEAPON_AXE | WEAPON_AXE | WEAPON_CROSSBOW | WEAPON_GLAIVE;
	m_iUItexNum = 20;
	m_eItemType = IT_RUNE;
	m_strFactoryTag = "PowerRune";
}

CPowerRune::~CPowerRune()
{
}

HRESULT CPowerRune::Ready_Object()
{
	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"PowerRuneUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return CRune::Ready_Object();
}

_int CPowerRune::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;
	return CRune::Update_Object(fTimeDelta);
}

void CPowerRune::EquipRune(CWeapon* pWeapon)
{
	m_iPrevDamage = pWeapon->GetDamage();
	pWeapon->SetDamage(static_cast<_uint>((_float)m_iPrevDamage * 1.5f));
	pWeapon->SetCritical(true);
}

void CPowerRune::UnEquipRune(CWeapon* pWeapon)
{
	pWeapon->SetDamage(m_iPrevDamage);
	pWeapon->SetCritical(false);
	m_iPrevDamage = 0;
}

CPowerRune* CPowerRune::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPowerRune* pInstance = new CPowerRune(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CPowerRune::Free()
{
	CRune::Free();
}

