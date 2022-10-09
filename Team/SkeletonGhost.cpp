#include "stdafx.h"
#include "..\Header\SkeletonGhost.h"

#include "Export_Function.h"

CSkeletonGhost::CSkeletonGhost(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_ePreState(MOTION_END)
	, m_eCurState(MOTION_END)
	, m_fHeight(0.f)
	, m_fTimeAcc(0.f)
	, m_fAttackTimeAcc(0.f)
{
}

CSkeletonGhost::~CSkeletonGhost()
{
}

HRESULT CSkeletonGhost::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_tInfo.iHp = 2;
	m_tInfo.iAttack = 1;

	m_fHeight = 3.f;
	m_pTransCom->Set_Pos(15.f, m_fHeight, 40.f);

	m_eCurState = IDLE;

	m_fIdle_Speed = 1.f;
	m_fAttack_Speed = 2.f;

	return S_OK;
}

_int CSkeletonGhost::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	m_pTransCom->Set_Y(m_fHeight);
	m_pAnimtorCom->Play_Animation(fTimeDelta);

	Target_Follow(fTimeDelta);
	Motion_Change(fTimeDelta);
	Circle();

	return 0;
}

void CSkeletonGhost::LateUpdate_Object(void)
{
	Billboard();
	//CMonster::Billboard();
	Engine::CGameObject::LateUpdate_Object();
}

void CSkeletonGhost::Render_Obejct(void)
{
	if(m_bCircle)
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	else
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pAnimtorCom->Set_Texture();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CSkeletonGhost::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// m_pAnimtorCom
	pComponent = m_pAnimtorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_AnimatorCom"));
	NULL_CHECK_RETURN(m_pAnimtorCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_AnimatorCom", pComponent });

	m_pAnimtorCom->Add_Component(L"Proto_SkeletonGhostIDLE_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_SkeletonGhostATTACK_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_SkeletonGhostHIT_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_SkeletonGhostDIE_Texture");

	return S_OK;
}

void CSkeletonGhost::Target_Follow(const _float & fTimeDelta)
{
	// 플레이어 따라가기
	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3		vPlayerPos, vPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	_float fDist = D3DXVec3Length(&(vPlayerPos - vPos));

	if (fDist < 7.f)
		m_bCircle = true;
	else
		m_bCircle = false;

	if (!m_bCircle && fDist < 15.f)
	{
		m_pTransCom->ChangeHeight_Target(&vPlayerPos, m_fHeight, m_fAttack_Speed, fTimeDelta);
	}
}

void CSkeletonGhost::Circle()
{
	if (!m_bCircle)
		return;

	_vec3	vPlayerPos;
	CTransform*		pPlayer = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK_RETURN(pPlayer);

	pPlayer->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransCom->Set_Pos(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);

	_vec3 vDir, vDistance;
	vDir = { 0.f, 1.f, 0.f };
	//D3DXVec3Normalize(&vDir, &vDir); // 단위 벡터로 변경
	D3DXVec3Normalize(&vDistance, &vDistance);

	_matrix matTrans, matRev;

	// 이
	D3DXMatrixTranslation(&matTrans, vDistance.x * 7.f, vDistance.y * -2.f, vDistance.z * 2.f);

	// 공
	m_fAngle += 0.01f;
	if (m_fAngle > 360.f)
		m_fAngle = 0.01f;
	D3DXMatrixRotationAxis(&matRev, &vDir, m_fAngle);

	_matrix matParent;
	pPlayer->Get_WorldMatrix(&matParent);

	_matrix mat;
	D3DXMatrixTranslation(&mat, matParent._41, matParent._42, matParent._43);

	matParent = matRev * mat;

	_matrix mat1;
	mat1 = matTrans * matParent; // 공전하는 행렬

	m_matWorld = mat1;

}

void CSkeletonGhost::Billboard()
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

	m_matWorld = matBill * m_matWorld;
}

void CSkeletonGhost::Motion_Change(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_pAnimtorCom->Change_Animation(L"Proto_SkeletonGhostIDLE_Texture");
			break;

		case ATTACK:
			m_pAnimtorCom->Change_Animation(L"Proto_SkeletonGhostATTACK_Texture");
			break;

		case HIT:
			m_pAnimtorCom->Change_Animation(L"Proto_SkeletonGhostHIT_Texture");
			break;

		case DIE:
			m_pAnimtorCom->Change_Animation(L"Proto_SkeletonGhostDIE_Texture");
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CSkeletonGhost * CSkeletonGhost::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkeletonGhost *	pInstance = new CSkeletonGhost(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSkeletonGhost::Free(void)
{
	CMonster::Free();
}
