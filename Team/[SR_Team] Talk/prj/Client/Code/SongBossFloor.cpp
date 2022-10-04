#include "stdafx.h"
#include "..\Header\SongBossFloor.h"

#include "Export_Function.h"	

CSongBossFloor::CSongBossFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet(pGraphicDev)
	, m_fSpeed(0.f)
{
}

CSongBossFloor::CSongBossFloor(const CSongBossFloor & rhs)
	:CBullet(rhs)
{
}

CSongBossFloor::~CSongBossFloor()
{
}

HRESULT CSongBossFloor::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Scale(0.3f, 0.3f, 0.3f);

	m_fSpeed = 5.f;

	return S_OK;
}

HRESULT CSongBossFloor::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// 버퍼 컴포넌트
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	// 월드행렬 컴포넌트
	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// m_pAnimtorCom
	pComponent = m_pAnimtorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_AnimatorCom"));
	NULL_CHECK_RETURN(m_pAnimtorCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_AnimatorCom", pComponent });

	m_pAnimtorCom->Add_Component(L"Proto_MusicNote_Floor_Texture");

	return S_OK;
}

_int CSongBossFloor::Update_Object(const _float & fTimeDelta)
{
	if (!m_bFire)
		return 0;

	int iResult = CGameObject::Update_Object(fTimeDelta);
	m_pAnimtorCom->Play_Animation(fTimeDelta);
	Add_RenderGroup(RENDER_ALPHA, this);

	if (!m_bReady)
	{
		CTransform*		pFist = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"SongBoss", L"Proto_TransformCom", ID_DYNAMIC));
		NULL_CHECK_RETURN(pFist, -1);

		pFist->Get_Info(INFO_POS, &vPos);
		m_pTransCom->Set_Pos(vPos.x, vPos.y, vPos.z); // 블렛의 시작 위치

		m_bReady = true;
	}
	CTransform*		pPlayer = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK_RETURN(pPlayer, -1);
	pPlayer->Get_Info(INFO_POS, &m_vPlayerPos); // 플레이어의 좌표를 받아와서

	// 몬스터와 플레이어의 거리를 구하고

	_vec3	vDir;
	vDir = m_vPlayerPos - vPos;
	D3DXVec3Normalize(&vDir, &vDir);
	vDir *= m_fSpeed * fTimeDelta;

	// 플레이어의 위치까지 간다
	m_pTransCom->Move_Pos(&vDir);

	_vec3 vTest, vTest2;
	m_pTransCom->Get_Info(INFO_POS, &vTest);
	pPlayer->Get_Info(INFO_POS, &vTest2);

	if (vTest < vTest2)
	{
		m_pTransCom->Set_Pos(vTest2.x + 2.f, vTest2.y , vTest2.z);
	}

	
	m_fLifeTime += fTimeDelta;
	return iResult;
}

void CSongBossFloor::LateUpdate_Object(void)
{
	Billboard();

	if (!m_bFire)
		return;

	//if (10.f < m_fLifeTime)
	//{
	//	Reset();
	//}

	CGameObject::LateUpdate_Object();
}

void CSongBossFloor::Render_Obejct(void)
{
	if (!m_bFire)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pAnimtorCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CSongBossFloor::Billboard()
{
	// 빌보드
	_matrix		matWorld, matView, matBill;
	D3DXMatrixIdentity(&matBill);

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// 스케일
	_vec3 vScale;
	m_pTransCom->Get_Scale(&vScale);

	_matrix matScale, matScaleInv;
	D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixInverse(&matScaleInv, 0, &matScale);

	// 현재 지금 이 코드는 문제가 없지만 나중에 문제가 될 수 있음
	m_pTransCom->Set_WorldMatrix(&(matScale * matBill * matScaleInv *  matWorld));
}

CSongBossFloor * CSongBossFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSongBossFloor*		pInstance = new CSongBossFloor(pGraphicDev);
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSongBossFloor::Free(void)
{
	CGameObject::Free();
}

void CSongBossFloor::Reset()
{
	m_bFire = false;
	m_bDead = false;
	m_fLifeTime = 0.f;
	m_fFrame = 0.f;
	m_bReady = false;
}