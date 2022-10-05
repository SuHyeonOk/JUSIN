#include "stdafx.h"
#include "..\Header\SongBosStun.h"

#include "Export_Function.h"	

CSongBosStun::CSongBosStun(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet(pGraphicDev)
{
}

CSongBosStun::CSongBosStun(const CSongBosStun & rhs)
	:CBullet(rhs)
{
}

CSongBosStun::~CSongBosStun()
{
}

HRESULT CSongBosStun::Ready_Object(_int iBulletCount)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_iBulletCount = iBulletCount;
	return S_OK;
}

HRESULT CSongBosStun::Add_Component(void)
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

	m_pAnimtorCom->Add_Component(L"Proto_MusicNote_Stun_Texture");

	return S_OK;
}

_int CSongBosStun::Update_Object(const _float & fTimeDelta)
{
	if (!m_bFire)
		return 0;

	int iResult = CGameObject::Update_Object(fTimeDelta);
	m_pAnimtorCom->Play_Animation(fTimeDelta);
	Add_RenderGroup(RENDER_ALPHA, this);

	// 플레이어 주변으로 생기는 음표
	CTransform*		pPlayer = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK_RETURN(pPlayer, -1);
	pPlayer->Get_Info(INFO_POS, &m_vPlayerPos); // 플레이어의 좌표를 받아와서

	_float fNotePos = 0.5f;

	if(m_iBulletCount == 0)
		m_pTransCom->Set_Pos(m_vPlayerPos.x + fNotePos, m_vPlayerPos.y - 0.03, m_vPlayerPos.z + fNotePos);
	else if (m_iBulletCount == 1)
		m_pTransCom->Set_Pos(m_vPlayerPos.x + fNotePos, m_vPlayerPos.y - 0.03, m_vPlayerPos.z - fNotePos);
	else if (m_iBulletCount == 2)
		m_pTransCom->Set_Pos(m_vPlayerPos.x - fNotePos, m_vPlayerPos.y - 0.03, m_vPlayerPos.z - fNotePos);
	else if (m_iBulletCount == 3)
		m_pTransCom->Set_Pos(m_vPlayerPos.x - fNotePos, m_vPlayerPos.y - 0.03, m_vPlayerPos.z + fNotePos);
	
	m_fLifeTime += fTimeDelta;
	return iResult;
}

void CSongBosStun::LateUpdate_Object(void)
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

void CSongBosStun::Render_Obejct(void)
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

void CSongBosStun::Billboard()
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

CSongBosStun * CSongBosStun::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iBulletCount)
{
	CSongBosStun*		pInstance = new CSongBosStun(pGraphicDev);
	if (FAILED(pInstance->Ready_Object(iBulletCount)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSongBosStun::Free(void)
{
	CGameObject::Free();
}

void CSongBosStun::Reset()
{
	m_bFire = false;
	m_bDead = false;
	m_fLifeTime = 0.f;
	m_fFrame = 0.f;
	m_bReady = false;
}