#include "stdafx.h"
#include "..\Header\SongBosFloor.h"

#include "Export_Function.h"	

CSongBosFloor::CSongBosFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet(pGraphicDev)
{
}

CSongBosFloor::CSongBosFloor(const CSongBosFloor & rhs)
	:CBullet(rhs)
{
}

CSongBosFloor::~CSongBosFloor()
{
}

HRESULT CSongBosFloor::Ready_Object(_int iBulletCount)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pTransCom->Rotation(ROT_X, 45.555f);

	m_iBulletCount = iBulletCount;
	
	return S_OK;
}

HRESULT CSongBosFloor::Add_Component(void)
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

_int CSongBosFloor::Update_Object(const _float & fTimeDelta)
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

	if (!m_bReady)
	{
		_float fNotePos = 0.5f;
		_float fNotePosY = 0.01f;

		if (m_iBulletCount == 0)
			m_pTransCom->Set_Pos(m_vPlayerPos.x, fNotePosY, m_vPlayerPos.z + 1.f);
		else if (m_iBulletCount == 1)
			m_pTransCom->Set_Pos(m_vPlayerPos.x + 1.f, fNotePosY, m_vPlayerPos.z);
		else if (m_iBulletCount == 2)
			m_pTransCom->Set_Pos(m_vPlayerPos.x - 1.f, fNotePosY, m_vPlayerPos.z);
		else if (m_iBulletCount == 3)
			m_pTransCom->Set_Pos(m_vPlayerPos.x, fNotePosY, m_vPlayerPos.z - 1.f);
		else if (m_iBulletCount == 4)
			m_pTransCom->Set_Pos(m_vPlayerPos.x, fNotePosY, m_vPlayerPos.z);

		m_bReady = true;
	}

	m_fLifeTime += fTimeDelta;
	return iResult;
}

void CSongBosFloor::LateUpdate_Object(void)
{
	if (!m_bFire)
		return;

	//if (10.f < m_fLifeTime)
	//{
	//	Reset();
	//}

	CGameObject::LateUpdate_Object();
}

void CSongBosFloor::Render_Obejct(void)
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

CSongBosFloor * CSongBosFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iBulletCount)
{
	CSongBosFloor*		pInstance = new CSongBosFloor(pGraphicDev);
	if (FAILED(pInstance->Ready_Object(iBulletCount)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSongBosFloor::Free(void)
{
	CGameObject::Free();
}

void CSongBosFloor::Reset()
{
	m_bFire = false;
	m_bDead = false;
	m_fLifeTime = 0.f;
	m_fFrame = 0.f;
	m_bReady = false;
}