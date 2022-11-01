#include "stdafx.h"
#include "..\Header\Axe.h"

#include "SkeletalCube.h"
#include "Player.h"
#include "Monster.h"
#include "StatComponent.h"
#include "TerrainCubeMap.h"
#include "Rune.h"
#include "Inventory.h"
#include "Kouku.h"
#include "StaticCamera.h"


CAxe::CAxe(LPDIRECT3DDEVICE9 pGraphicDev): CWeapon(pGraphicDev)
{
	m_eType = WEAPON_AXE;
	m_iDamage = 50;
	m_strFactoryTag = "Axe";
}

CAxe::~CAxe()
{
}

HRESULT CAxe::Ready_Object()
{
	FAILED_CHECK_RETURN(CWeapon::Ready_Object(), E_FAIL);

	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CVoxelTex>(L"Proto_VoxelTex_Axe", L"Proto_VoxelTex_Axe", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_WeaponTexture", L"Proto_WeaponTexture", ID_STATIC);

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_idle.anim");
	m_arrAnim[ANIM_IDLE].bLoop = true;
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_walk.anim");
	m_arrAnim[ANIM_WALK].bLoop = true;
	m_arrAnim[ANIM_ATTACK1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/axe_attack_a.anim");
	m_arrAnim[ANIM_ATTACK2] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/axe_attack_b.anim");
	m_arrAnim[ANIM_ROLL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/roll.anim");
	m_arrAnim[ANIM_RANGE_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_start.anim");
	m_arrAnim[ANIM_LEGACY1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/shock_powder.anim");
	m_arrAnim[ANIM_LEGACY2] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/shock_powder.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/dead.anim");

	m_eItemType = IT_MELEE;
	m_iUItexNum = 7;

	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"AxeUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);
	return S_OK;
}

_int CAxe::Update_Object(const _float& fTimeDelta)
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

	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	_float fHeight = pCubeMap->GetHeight(vPos.x, vPos.z);

	Parabola(vPos, fHeight, fTimeDelta);

	CWeapon::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CAxe::Render_Object()
{
	if (m_eItemState == IS_TAKE)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(3);
	m_pBufferCom->Render_Buffer();
}

CAxe* CAxe::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAxe* pInstance = new CAxe(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

_int CAxe::Attack()
{
	CPlayer* pPlayer = m_pOwner;
	if (pPlayer == nullptr)
		return 0;

	if (m_iAttackCnt == 0)
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK1]);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_axeSwingSwong-001.ogg", pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS]);
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayShake(0.1f, 0.1f);
	}
	else if (m_iAttackCnt == 1)
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK2]);
		CSoundMgr::GetInstance()->PlaySound(L"sfx_Axe_2.ogg", pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS]);
	}
	m_iAttackCnt = (m_iAttackCnt + 1) % 2;

	return m_iAttackCnt;
}

void CAxe::Equipment(SkeletalPart* pSkeletalPart)
{
	pSkeletalPart->pBuf = m_pBufferCom;
	pSkeletalPart->pTex = m_pTextureCom;
	pSkeletalPart->iTexIdx = 3;
}

void CAxe::Collision()
{
	set<CGameObject*> objSet;

	CPlayer* pPlayer = m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);
	_vec3 vLook = pPlayer->GetInfo(INFO_LOOK);

	_vec3 vAttackPos = vPos;
	if (m_iAttackCnt == 0)
		vAttackPos += vLook * 2.5;

	Engine::GetOverlappedObject(OUT objSet, vAttackPos, 3.f);
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

	DEBUG_SPHERE(vAttackPos, 3.f, 1.f);
}

void CAxe::Free()
{
	CWeapon::Free();
}
