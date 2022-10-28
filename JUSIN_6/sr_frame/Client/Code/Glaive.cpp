#include "stdafx.h"
#include "..\Header\Glaive.h"
#include "SkeletalCube.h"
#include "Player.h"
#include "StatComponent.h"
#include "Monster.h"
#include "TerrainCubeMap.h"
#include "Rune.h"
#include "Inventory.h"
#include "Kouku.h"

CGlaive::CGlaive(LPDIRECT3DDEVICE9 pGraphicDev)
	:CWeapon(pGraphicDev)
{
	m_eType = WEAPON_GLAIVE;
	m_iDamage = 30;
	m_strFactoryTag = "Glaive";
}



CGlaive::~CGlaive()
{
}

HRESULT CGlaive::Ready_Object()
{
	FAILED_CHECK_RETURN(CWeapon::Ready_Object(), E_FAIL);
	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CVoxelTex>(L"Proto_VoxelTex_Glaive", L"Proto_VoxelTex_Glaive", ID_STATIC);
	m_pTextureCom = Add_Component<Engine::CTexture>(L"Proto_WeaponTexture", L"Proto_WeaponTexture", ID_STATIC);

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/glaive_idle.anim");
	m_arrAnim[ANIM_IDLE].bLoop = true;
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/glaive_walk.anim");
	m_arrAnim[ANIM_WALK].bLoop = true;
	m_arrAnim[ANIM_ATTACK1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/glaive_attack_a_jjh.anim");
	m_arrAnim[ANIM_ATTACK2] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/glaive_attack_b_jjh.anim");
	m_arrAnim[ANIM_ATTACK3] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/glavie_attack_b.anim");
	m_arrAnim[ANIM_ROLL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/roll.anim");
	m_arrAnim[ANIM_RANGE_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_start.anim");
	m_arrAnim[ANIM_LEGACY1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/shock_powder.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/dead.anim");

	m_eItemType = IT_MELEE;
	m_iUItexNum = 9;

	_float fItemSpaceSize = WINCX * 0.08f;
	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"GlaiveUI", 0/*, WINCX * 0.4f, WINCY*0.25f, fItemSpaceSize, fItemSpaceSize*/);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return S_OK;
}

_int CGlaive::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	//runeslot on/off
	if (m_pInventory->GetCurClickItem() == this)
	{
		if (m_pRune != nullptr && m_pRune->GetItemUI())
		{
			m_pRune->GetItemUI()->Open();
		}
	}
	else
	{
		if (m_pRune != nullptr && m_pRune->GetItemUI())
		{
			m_pRune->GetItemUI()->Close();			
		}
	}


	if (m_eItemState == IS_TAKE)
		return 0;

	m_pRune == nullptr ? m_pRune->UnEquipRune(this) : m_pRune->EquipRune(this);

	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	_float fHeight = pCubeMap->GetHeight(vPos.x, vPos.z);

	Parabola(vPos, fHeight, fTimeDelta);

	
	CWeapon::Update_Object(fTimeDelta);


	return 0;
}

void CGlaive::LateUpdate_Object()
{
	CEquipItem::LateUpdate_Object();

}

void CGlaive::Render_Object()
{
	if (m_eItemState == IS_TAKE)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(1);
	m_pBufferCom->Render_Buffer();
}

_int CGlaive::Attack()
{
	CPlayer* pPlayer = m_pOwner;
	if (pPlayer == nullptr)
		return 0;

	if (m_iAttackCnt == 2)
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK3]);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_glaiveSwing-001.ogg", pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS]);
	}
	else if(m_iAttackCnt == 1)
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK2]);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_glaiveSwing-002.ogg", pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS]);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_glaiveSwing-003.ogg", pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS]);
	}
	else
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK1]);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_glaiveSwing-004.ogg", pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS]);
	}

	m_iAttackCnt = (m_iAttackCnt + 1) % 3;
	return m_iAttackCnt;
}

CGlaive * CGlaive::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGlaive* pInstance = new CGlaive(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CGlaive::Free()
{
	CWeapon::Free();
}

void CGlaive::Equipment(SkeletalPart * pSkeletalPart)
{
	pSkeletalPart->pBuf = m_pBufferCom;
	pSkeletalPart->pTex = m_pTextureCom;
	pSkeletalPart->iTexIdx = 1;
}

void CGlaive::Collision()
{
	set<CGameObject*> objSet;

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);
	_vec3 vLook = pPlayer->GetInfo(INFO_LOOK);

	_vec3 vAttackPos = vPos + (vLook * 2.5f);
	Engine::GetOverlappedObject(OUT objSet, vAttackPos, 2.5f);
	for (auto& obj : objSet)
	{
		if (CMonster* monster = dynamic_cast<CMonster*>(obj))
		{
			if (CKouku* pKouku = dynamic_cast<CKouku*>(obj))
			{
				if (!pKouku->Kouku_Stun() && m_iAttackCnt == 0 && pKouku->Kouku_Countable())
				{
					pKouku->Kouku_Stun_Success(m_pOwner->GetID());
				}
			}
			DamageType eDT = DT_END;
			if (m_iAttackCnt == 0) eDT = DT_KNOCK_BACK;
			if (monster->CheckCC()) eDT = DT_END;
			monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(m_iDamage, vPos, m_pOwner, eDT, m_bCritical);
		}
	}

	if (m_pRune)
		m_pRune->Collision();

	DEBUG_SPHERE(vAttackPos, 2.5f, 1.f);
}
