#include "stdafx.h"
#include "..\Header\Sword.h"
#include "SkeletalCube.h"
#include "Player.h"
#include "Monster.h"
#include "Rune.h"
#include "StatComponent.h"
#include "TerrainCubeMap.h"
#include "Rune.h"
#include "Inventory.h"
#include "Kouku.h"
#include "Service.h"


CSword::CSword(LPDIRECT3DDEVICE9 pGraphicDev)
	:CWeapon(pGraphicDev)
{
	m_eType = WEAPON_SWORD;
	m_iDamage = 25;
	m_strFactoryTag = "Sword";
}

CSword::~CSword()
{
}

HRESULT CSword::Ready_Object()
{
	FAILED_CHECK_RETURN(CWeapon::Ready_Object(), E_FAIL);

	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CVoxelTex>(L"Proto_VoxelTex_Sword", L"Proto_VoxelTex_Sword", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_WeaponTexture", L"Proto_WeaponTexture", ID_STATIC);

	m_arrAnim[ANIM_IDLE] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_idle.anim");
	m_arrAnim[ANIM_IDLE].bLoop = true;
	m_arrAnim[ANIM_WALK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_walk.anim");
	m_arrAnim[ANIM_WALK].bLoop = true;
	m_arrAnim[ANIM_ATTACK1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_attack_a.anim");
	m_arrAnim[ANIM_ATTACK2] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_attack_b.anim");
	m_arrAnim[ANIM_ATTACK3] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/sword_attack_c.anim");
	m_arrAnim[ANIM_ROLL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/roll.anim");
	m_arrAnim[ANIM_RANGE_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_start.anim");
	m_arrAnim[ANIM_LEGACY1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/shock_powder.anim");
	m_arrAnim[ANIM_LEGACY2] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/shock_powder.anim");
	m_arrAnim[ANIM_DEAD] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/dead.anim");

	m_eItemType = IT_MELEE;
	m_iUItexNum = 10;

	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"SwordUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return S_OK;
}

_int CSword::Update_Object(const _float & fTimeDelta)
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

	if (m_bIdle == true && !m_bCreateOnce)
	{
		CGradation_Beam* pBeam = nullptr;

		pBeam = CEffectFactory::Create<CGradation_Beam>("Gradation_Beam", L"Gradation_Beam");
		Get_GameObject<C3DBaseTexture>(LAYER_EFFECT, L"3D_Base")->Add_Particle(m_pTransCom->m_vInfo[INFO_POS], 3.f, D3DXCOLOR(1.f, 1.f, 0.f, 0.f), 1, 30.f, 1);
		pBeam->SetTransform(m_pTransCom->m_vInfo[INFO_POS]);
		m_bCreateOnce = true;
	}
	

	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	_float fHeight = pCubeMap->GetHeight(vPos.x, vPos.z);

	Parabola(vPos, fHeight, fTimeDelta);

	CWeapon::Update_Object(fTimeDelta);


	return 0;
}

void CSword::LateUpdate_Object()
{
	CEquipItem::LateUpdate_Object();
}

void CSword::Render_Object()
{
	//runeslot on/off
	if (m_pInventory->GetCurClickItem() == this)
	{
		if (m_pRune != nullptr)
		{
			m_pRune->GetItemUI()->Open();
			m_pInventory->SetRune(m_pRune);
		}
	}
	else
	{
		if (m_pRune != nullptr)
		{
			m_pRune->GetItemUI()->Close();
			m_pInventory->SetRune(nullptr);
		}
	}


	if (m_eItemState == IS_TAKE)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();
}

CSword * CSword::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSword* pInstance = new CSword(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSword::Free()
{
	CWeapon::Free();
}

_int CSword::Attack()
{
	CPlayer* pPlayer = m_pOwner;

	if (pPlayer == nullptr)
		return 0;

	if (m_iAttackCnt == 0)
	{
		
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK1]);
	}
	else if (m_iAttackCnt == 1)
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK2]);
	}
	else
	{
		pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK3]);
	}
	CSoundMgr::GetInstance()->PlaySoundRandom({L"sfx_item_swordSwingSteel-001_soundWave.ogg", L"sfx_item_swordSwingSteel-002_soundWave.ogg", L"sfx_item_swordSwingSteel-003_soundWave.ogg"}, 
		pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS], 0.4f);
	m_iAttackCnt = (m_iAttackCnt + 1) % 3;

	return m_iAttackCnt;
}

void CSword::Equipment(SkeletalPart * pSkeletalPart)
{
	pSkeletalPart->pBuf = m_pBufferCom;
	pSkeletalPart->pTex = m_pTextureCom;
	pSkeletalPart->iTexIdx = 0;
}

void CSword::Collision()
{
	set<CGameObject*> objSet;

	CPlayer* pPlayer =m_pOwner;
	_vec3 vPos = pPlayer->GetInfo(INFO_POS);
	_vec3 vLook = pPlayer->GetInfo(INFO_LOOK);
	
	_vec3 vAttackPos = vPos + (vLook * 2.f);
	Engine::GetOverlappedObject(OUT objSet, vAttackPos, 2.f);
	for (auto& obj : objSet)
	{
		if (CMonster* monster = dynamic_cast<CMonster*>(obj))
		{
			DamageType eDT = DT_END;
			if(CKouku* pKouku = dynamic_cast<CKouku*>(obj))
			{
				if(!pKouku->Kouku_Stun() && m_iAttackCnt == 0 && pKouku->Kouku_Countable())
				{
					pKouku->Kouku_Stun_Success(m_pOwner->GetID());

				}
			}
			if (m_iAttackCnt == 0) eDT = DT_KNOCK_BACK;
			
			if (monster->CheckCC()) eDT = DT_END;
			monster->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
				->TakeDamage(m_iDamage, vPos, m_pOwner, eDT, m_bCritical);
		}
	}

	if (m_pRune)
		m_pRune->Collision();

	DEBUG_SPHERE(vAttackPos, 2.f, 1.f);
}
