#include "stdafx.h"
#include "BootsOfSwiftness.h"
#include "Particle.h"
#include "AbstFactory.h"
#include "Player.h"
#include "SkeletalGhostTrail.h"

CBootsOfSwiftness::CBootsOfSwiftness(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEquipItem(pGraphicDev)
{
	m_strFactoryTag = "BootsOfSwiftness";
}


CBootsOfSwiftness::~CBootsOfSwiftness()
{
}

HRESULT CBootsOfSwiftness::Ready_Object()
{
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_iUItexNum = 11;
	m_fCurCoolTime = 10.f;
	m_fCoolTime = 10.f;
	m_fLifeTime = 8.f;

	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"BootsUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);

	return S_OK;
}

_int CBootsOfSwiftness::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	if (m_bEnd)
		m_fCurTrailTime += fTimeDelta;
	if (m_bEnd && m_fCurTrailTime > m_fTrailTime)
	{
		m_fCurTrailTime = 0.f;

		CTransform* pOwnerTrans = m_pOwner->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC);
		_matrix matWorld;
		CGameUtilMgr::MatWorldComposeEuler(matWorld, {0.85f, 0.85f, 0.85f}, pOwnerTrans->m_vAngle, pOwnerTrans->m_vInfo[INFO_POS]);
		CObjectFactory::CreateGhostTrail("GhostTrail", L"GhostTrail", m_pOwner, matWorld)
			->SetColorTime(0.7f, D3DCOLOR_ARGB(150, 000, 204, 255));
	}

	if (m_fCoolTime > m_fCurCoolTime)
	{
		m_fCurCoolTime += fTimeDelta;
		m_bUse = false;
		return OBJ_NOEVENT;
	}

	if (m_bUse)
	{
		m_fCurCoolTime = 0.f;
		m_fAge = 0.f;
	
		CPlayer* pPlayer = m_pOwner;
		pPlayer->SetSpeed(7.5f);
		_vec3 vPos = pPlayer->GetInfo(INFO_POS);

		CSoundMgr::GetInstance()->PlaySound(L"sfx_item_glaiveSwing-004.ogg", vPos);
		m_bEnd = true;
		m_bUse = false;
	}


	m_fAge += fTimeDelta;

	if (m_bEnd && m_fAge >= m_fLifeTime)
	{
		m_pOwner->SetSpeed(4.5f);
		m_bEnd = false;
	}


	CEquipItem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CBootsOfSwiftness::LateUpdate_Object()
{
	CEquipItem::LateUpdate_Object();
}

void CBootsOfSwiftness::Render_Object()
{
}

void CBootsOfSwiftness::Free()
{
	CEquipItem::Free();
}

CBootsOfSwiftness * CBootsOfSwiftness::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBootsOfSwiftness* pInstance = new CBootsOfSwiftness(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBootsOfSwiftness::Equipment(SkeletalPart * pSkeletalPart)
{
}
