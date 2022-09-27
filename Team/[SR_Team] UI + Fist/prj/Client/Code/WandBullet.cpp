#include "stdafx.h"
#include "..\Header\WandBullet.h"

#include "Export_Function.h"	

CWandBullet::CWandBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet(pGraphicDev)
{
	//m_pGraphicDev->AddRef();
}

CWandBullet::CWandBullet(const CWandBullet & rhs)
	:CBullet(rhs)
{
	//m_pGraphicDev->AddRef();
}

CWandBullet::~CWandBullet()
{
}

HRESULT CWandBullet::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->Set_Scale(0.05f, 0.05f, 0.05f);
	return S_OK;
}

HRESULT CWandBullet::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// 버퍼 컴포넌트
	pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Clone_Proto(L"Proto_CubeTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTexCom", pComponent });

	// 텍스쳐 컴객체 컴포넌트
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_CubeTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTexture", pComponent });

	// 월드행렬 컴포넌트
	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	return S_OK;
}

_int CWandBullet::Update_Object(const _float & fTimeDelta)
{
	if (!m_bFire)
		return 0;

	if (!m_bReady)
	{
		CTransform*		pPlayer = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
		NULL_CHECK_RETURN(pPlayer, -1);

		_vec3 vPos;
		pPlayer->Get_Info(INFO_POS, &vPos);
		m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z);

		pPlayer->Get_Info(INFO_LOOK, &m_vDirection);
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_bReady = true;
	}


	m_pTransCom->Move_Pos(&(m_fSpeed * fTimeDelta * m_vDirection));

	int iResult = CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);
	
	m_fLifeTime += fTimeDelta;
	
	return iResult;
}

void CWandBullet::LateUpdate_Object(void)
{
	if (!m_bFire)
		return;

	if (1.f < m_fLifeTime)
	{
		Reset();
	}

	CGameObject::LateUpdate_Object();
}

void CWandBullet::Render_Obejct(void)
{
	if (!m_bFire)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(_ulong(m_fFrame));

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

}

CWandBullet * CWandBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWandBullet*		pInstance = new CWandBullet(pGraphicDev);
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWandBullet::Free(void)
{
	CGameObject::Free();
	//Safe_Release(m_pGraphicDev);
}

void CWandBullet::Reset()
{
	m_bFire = false;
	m_bDead = false;
	m_fLifeTime = 0.f;
	m_fFrame = 0.f;
	m_bReady = false;
}

