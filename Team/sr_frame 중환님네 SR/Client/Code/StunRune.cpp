#include "stdafx.h"
#include "..\Header\StunRune.h"
#include "Player.h"
#include "Monster.h"
#include "StatComponent.h"

CStunRune::CStunRune(LPDIRECT3DDEVICE9 pGraphicDev) : CRune(pGraphicDev)
{
	m_eTargetType = WEAPON_AXE;
	m_iUItexNum = 19;
	m_eItemType = IT_RUNE;
	m_strFactoryTag = "StunRune";
}

CStunRune::~CStunRune()
{
}

HRESULT CStunRune::Ready_Object()
{
	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"StunRuneUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return CRune::Ready_Object();
}

_int CStunRune::Update_Object(const _float& fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	return CRune::Update_Object(fTimeDelta);
}

void CStunRune::LateUpdate_Object()
{
	CRune::LateUpdate_Object();
}

void CStunRune::Render_Object()
{
	CRune::Render_Object();
}

void CStunRune::Free()
{
	CRune::Free();
}

void CStunRune::Collision()
{
	set<CGameObject*> objSet;

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);
	_vec3 vLook = pPlayer->GetInfo(INFO_LOOK);

	_vec3 vAttackPos = vPos + vLook * 2.5f;

	Engine::GetOverlappedObject(OUT objSet, vAttackPos, 3.f);
	for (auto& obj : objSet)
	{
		if (CMonster* monster = dynamic_cast<CMonster*>(obj))
		{
			DamageType eDT = DT_STUN;
			if (monster->CheckCC()) eDT = DT_END;
			monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(0, vPos, this, eDT);
		}
	}
}

CStunRune* CStunRune::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStunRune* pInstance = new CStunRune(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

