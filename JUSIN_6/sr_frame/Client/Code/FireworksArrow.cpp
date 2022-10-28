#include "stdafx.h"
#include "..\Header\FireworksArrow.h"
#include "Crossbow.h"

CFireworksArrow::CFireworksArrow(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEquipItem(pGraphicDev)
{
	m_strFactoryTag = "FireworksArrow";
}


CFireworksArrow::~CFireworksArrow()
{
}

HRESULT CFireworksArrow::Ready_Object()
{
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_iUItexNum = 13;
	m_fCurCoolTime = 8.f;
	m_fCoolTime = 8.f;

	m_pItemUI = CUIFactory::Create<CItemUI>("ItemUI", L"FireWorkUI", 0);
	m_pItemUI->SetUITexture(m_iUItexNum);
	return S_OK;
}

_int CFireworksArrow::Update_Object(const _float & fTimeDelta)
{
	if (m_bDelete) return OBJ_DEAD;

	if (m_fCoolTime > m_fCurCoolTime)
	{
		m_fCurCoolTime += fTimeDelta;
		m_bUse = false;
		return OBJ_NOEVENT;
	}

	if (!m_bUse)
		return 0;
	m_fCurCoolTime = 0.f;

	CCrossbow* pCrossbow = Get_GameObject<CCrossbow>(LAYER_ITEM, L"Crossbow");
	pCrossbow->LoadFireWork();

	m_bUse = false;

	CEquipItem::Update_Object(fTimeDelta);

	return OBJ_NOEVENT;
}

void CFireworksArrow::LateUpdate_Object()
{
	CEquipItem::LateUpdate_Object();
}

void CFireworksArrow::Render_Object()
{
}

void CFireworksArrow::Free()
{
	CEquipItem::Free();
}

CFireworksArrow * CFireworksArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFireworksArrow* pInstance = new CFireworksArrow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CFireworksArrow::Equipment(SkeletalPart * pSkeletalPart)
{
}
