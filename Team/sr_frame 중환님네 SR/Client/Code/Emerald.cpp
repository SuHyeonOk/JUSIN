#include "stdafx.h"
#include "..\Header\Emerald.h"
#include "TerrainCubeMap.h"
#include "Player.h"
#include "Inventory.h"

CEmerald::CEmerald(LPDIRECT3DDEVICE9 pGraphicDev)
	:CConsumeItem(pGraphicDev)
{
}


CEmerald::~CEmerald()
{
}

HRESULT CEmerald::Ready_Object()
{
	m_pTextureCom = Add_Component<CTexture>(L"Proto_Emerald", L"Proto_Emerald", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);

	m_eItemEffect = IE_MONEY;
	return S_OK;
}

_int CEmerald::Update_Object(const _float & fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	if (m_eItemState == IS_TAKE)
		return 0;

	_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];
	CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	_float fHeight = pCubeMap->GetHeight(vPos.x, vPos.z);

	Parabola(vPos, fHeight, fTimeDelta);

	if (m_bIdle)
	{
		CPlayer* pPlayer = Get_GameObject<CPlayer>(LAYER_PLAYER, L"Player");
		_vec3 vPlayerPos = pPlayer->GetInfo(INFO_POS);
		_vec3 vDest = vPlayerPos - m_pTransCom->m_vInfo[INFO_POS];
		_float vDist = D3DXVec3Length(&vDest);
		D3DXVec3Normalize(&vDest, &vDest);
		m_pTransCom->m_vInfo[INFO_POS] += vDest * 13.f * fTimeDelta;

		if (vDist < 1.f)
			pPlayer->GetInventory()->Put(this);
	}


	CConsumeItem::Update_Object(fTimeDelta);
	return OBJ_NOEVENT;

}

void CEmerald::LateUpdate_Object()
{
	CConsumeItem::LateUpdate_Object();
}

void CEmerald::Render_Object()
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

void CEmerald::Free()
{
	CConsumeItem::Free();
}

CEmerald * CEmerald::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEmerald* pInstance = new CEmerald(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CEmerald::Parabola(_vec3 & vPos, const _float & fHeight, const _float & fTimeDelta)
{

	if (vPos.y <= fHeight || m_bIdle)
	{
		if (!m_bIdle)
		{
			m_fTime = 0.5f;
			vPos.y = fHeight + 0.5f;
		}


		if (vPos.y <= (1.5f + fHeight) && vPos.y >= fHeight + 0.5f)
		{
			vPos.y += m_fTime * fTimeDelta;
		}
		else
		{
			if (vPos.y >= 1.f + fHeight)
				vPos.y = 1.5f + fHeight;
			else
				vPos.y =  fHeight + 0.5f;

			m_fTime *= -1.f;
		}

		m_bIdle = true;
	}
	else
	{
		vPos.y += m_fPower * m_fTime * fTimeDelta - (9.8f * m_fTime * m_fTime * fTimeDelta * 0.5f);
		vPos.x += 3.f * m_vVelocity.x * fTimeDelta;
		vPos.z += 3.f * m_vVelocity.z * fTimeDelta;
		m_fTime += 7.f * fTimeDelta;
	}
}
