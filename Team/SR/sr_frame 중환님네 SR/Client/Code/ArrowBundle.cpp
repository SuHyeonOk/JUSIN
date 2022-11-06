#include "stdafx.h"
#include "..\Header\ArrowBundle.h"
#include "TerrainCubeMap.h"

CArrowBundle::CArrowBundle(LPDIRECT3DDEVICE9 pGraphicDev)
	:CConsumeItem(pGraphicDev)
{
}


CArrowBundle::~CArrowBundle()
{
}

HRESULT CArrowBundle::Ready_Object()
{
	m_pTextureCom = Add_Component<CTexture>(L"Proto_ArrowBundle", L"Proto_ArrowBundle", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);

	m_eItemEffect = IE_ARROW;
	return S_OK;
}

_int CArrowBundle::Update_Object(const _float & fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	if (m_eItemState == IS_TAKE)
		return 0;

	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	_float fHeight = pCubeMap->GetHeight(vPos.x, vPos.z);

	Parabola(vPos, fHeight, fTimeDelta);

	CConsumeItem::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;
}

void CArrowBundle::LateUpdate_Object()
{
	CConsumeItem::LateUpdate_Object();
}

void CArrowBundle::Render_Object()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CArrowBundle::Free()
{
	CConsumeItem::Free();
}

CArrowBundle * CArrowBundle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CArrowBundle* pInstance = new CArrowBundle(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
