#include "stdafx.h"
#include "..\Header\Crossbow.h"
#include "AbstFactory.h"
#include "Player.h"
#include "TerrainCubeMap.h"
#include "Rune.h"
#include "Inventory.h"
#include "PowerRune.h"

CCrossbow::CCrossbow(LPDIRECT3DDEVICE9 pGraphicDev)
	:CWeapon(pGraphicDev)
	
{
	m_eType = WEAPON_CROSSBOW;
	m_iDamage = 20;
	m_strFactoryTag = "Crossbow";
}


CCrossbow::~CCrossbow()
{
}

HRESULT CCrossbow::Ready_Object()
{
	FAILED_CHECK_RETURN(CWeapon::Ready_Object(), E_FAIL);
	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CVoxelTex>(L"Proto_VoxelTex_Crossbow", L"Proto_VoxelTex_Crossbow", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_WeaponTexture", L"Proto_WeaponTexture", ID_STATIC);

	//Attack_start
	m_arrAnim[ANIM_ATTACK1] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_start.anim");
	//Attack_roof
	m_arrAnim[ANIM_ATTACK2] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_loop.anim");
	//Attack_end
	m_arrAnim[ANIM_ATTACK3] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_end.anim");
	m_arrAnim[ANIM_ROLL] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/roll.anim");
	m_arrAnim[ANIM_RANGE_ATTACK] = CubeAnimFrame::Load(L"../Bin/Resource/CubeAnim/CubeMan/crossbow_attack_start.anim");

	m_eItemType = IT_RANGE;
	m_iUItexNum = 8;

	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"CrossBowUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);
	return S_OK;
}

_int CCrossbow::Update_Object(const _float & fTimeDelta)
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

	return 0;
}

void CCrossbow::LateUpdate_Object()
{
	CEquipItem::LateUpdate_Object();
}

void CCrossbow::Render_Object()
{
	if (m_eItemState == IS_TAKE)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(2);
	m_pBufferCom->Render_Buffer();
}

CCrossbow * CCrossbow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCrossbow* pInstance = new CCrossbow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCrossbow::Free()
{
	CWeapon::Free();
}

_int CCrossbow::Attack()
{
	CPlayer* pPlayer = m_pOwner;
	if (pPlayer == nullptr)
		return 0;

	pPlayer->PlayAnimationOnce(&m_arrAnim[ANIM_ATTACK1]);


	if (m_pRune == nullptr || dynamic_cast<CPowerRune*>(m_pRune))
	{
		if (m_bFireWork)
		{
			m_pOwner->SpawnArrow(50, PlayerArrowType::NORMAL, m_bCritical, ARROW_FIREWORK);
			m_bFireWork = false;
		}
		else
		{
			m_pOwner->SpawnArrow(m_iDamage, PlayerArrowType::NORMAL, m_bCritical);
		}
	}
	else
	{
		m_pRune->Use();
	}


	m_bFireWork = false;
	return m_iAttackCnt;
}

void CCrossbow::Equipment(SkeletalPart* pSkeletalPart)
{
	pSkeletalPart->pBuf = m_pBufferCom;
	pSkeletalPart->pTex = m_pTextureCom;
	pSkeletalPart->iTexIdx = 2;
	m_iAttackCnt = 0;
}

