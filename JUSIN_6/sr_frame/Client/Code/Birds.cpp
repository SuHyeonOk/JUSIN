#include "stdafx.h"
#include "..\Header\Birds.h"

#include "Player.h"
#include "StaticCamera.h"

CBirds::CBirds(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{

}

CBirds::~CBirds()
{
}

HRESULT CBirds::Ready_Object(void)
{
	m_pBufferCom = Add_Component<CRcTex>(L"Proto_BirdWhite", L"Proto_BirdWhite", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_BirdsWhiteTextCom", L"Proto_BirdsWhiteTextCom", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_fSpeed = 10.f;

	return S_OK;
}

_int CBirds::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	if (m_bStart == false && m_fCurTriggerFreq < m_fTriggerFreq)
		m_fCurTriggerFreq += fTimeDelta;

	m_fFrame += 8.f * fTimeDelta;

	if (m_bStart == false)
	{
		m_fFrame = 0.f;
	}

	if (m_bStart)
	{
		if (m_fFrame >= 5.f)
			m_fFrame = 1.f;

		m_pTransCom->m_vInfo[INFO_POS] += fTimeDelta * m_vDir * m_fSpeed;

		const _float fDist = D3DXVec3Length(&(m_pTransCom->m_vInfo[INFO_POS] - m_vPlayerPos));

		if (fDist > 30.f)
			return OBJ_DEAD;
	}

	for (auto e : Get_Layer(LAYER_ENV)->Get_MapObject())
	{
		if (CStaticCamera* pStatic = dynamic_cast<CStaticCamera*>(e.second))
		{
			m_pTransCom->m_vAngle.y = pStatic->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)
				->m_vAngle.y;
			if (m_bReverse) m_pTransCom->m_vAngle.y += D3DXToRadian(180.f);
		}
	}

	Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CBirds::LateUpdate_Object(void)
{
	if (m_fCurTriggerFreq >= m_fTriggerFreq)
	{
		set<CGameObject*> setObj;
		Engine::GetOverlappedObject(setObj, m_pTransCom->m_vInfo[INFO_POS], 5.f);

		for (auto obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				m_vPlayerPos = pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_vInfo[INFO_POS];
				m_bStart = true;

				m_vDir = { CGameUtilMgr::GetRandomFloat(-1.f, 1.f), 0.5f, CGameUtilMgr::GetRandomFloat(-1.f, 1.f) };
				D3DXVec3Normalize(&m_vDir, &m_vDir);

				_matrix matCamWorld;
				m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
				D3DXMatrixInverse(&matCamWorld, nullptr, &matCamWorld);
				
				_vec3 vCamLook{ matCamWorld._31, matCamWorld._32, matCamWorld._33};
				D3DXVec3Normalize(&vCamLook, &vCamLook);
				
				_vec3 vCross;
				D3DXVec3Cross(&vCross, &vCamLook, &m_vDir);
				
				if (vCross.y < 0.f)
					m_bReverse = true;
			}
		}
		m_fCurTriggerFreq = 0.f;
	}
}

void CBirds::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	

	m_pTextureCom->Set_Texture((_ulong)m_fFrame);

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


CBirds * CBirds::Create(LPDIRECT3DDEVICE9 pGraphicDev, OBJTYPE eType)
{
	CBirds*		pInstance = new CBirds(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBirds::Free(void)
{
	CGameObject::Free();
}
