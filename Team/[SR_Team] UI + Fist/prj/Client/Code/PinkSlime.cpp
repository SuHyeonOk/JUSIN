#include "stdafx.h"
#include "..\Header\PinkSlime.h"

#include "Export_Function.h"

CPinkSlime::CPinkSlime(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_eCurState(MOTION_END)
	, m_fIdleSpeed(0.f)
	, m_fTimeAcc(0.f)

{
}


CPinkSlime::~CPinkSlime()
{
}

HRESULT CPinkSlime::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Pos(5.f, 0.f, 5.f);

	m_pTransCom->Set_Y(1.f);

	m_eCurState = IDLE;

	m_fSpeed = 2.f;
	m_fIdleSpeed = 1.f;

	return S_OK;
}

_int CPinkSlime::Update_Object(const _float & fTimeDelta)
{
	//m_eCurState = ATTACK;

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pAnumtorCom->Play_Animation(fTimeDelta);

	Motion_Change(fTimeDelta);

	//Target_Follow(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CPinkSlime::LateUpdate_Object(void)
{
	CMonster::Billboard();
	Engine::CGameObject::LateUpdate_Object();
}

void CPinkSlime::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pAnumtorCom->Set_Texture();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CPinkSlime::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// m_pAnumtorCom
	pComponent = m_pAnumtorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_AnimatorCom"));
	NULL_CHECK_RETURN(m_pAnumtorCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_AnimatorCom", pComponent });

	m_pAnumtorCom->Add_Component(L"Proto_PinkSlimeIDLE_Texture");
	m_pAnumtorCom->Add_Component(L"Proto_PinkSlimeATTACK_Texture");
	m_pAnumtorCom->Add_Component(L"Proto_PinkSlimeHIT_Texture");
	m_pAnumtorCom->Add_Component(L"Proto_PinkSlimeDIE_Texture");

	return S_OK;
}

CPinkSlime * CPinkSlime::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPinkSlime *	pInstance = new CPinkSlime(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPinkSlime::Free(void)
{
	CMonster::Free();
}

void CPinkSlime::Motion_Change(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_pAnumtorCom->Change_Animation(L"Proto_PinkSlimeIDLE_Texture");
			break;

		case ATTACK:
			m_pAnumtorCom->Change_Animation(L"Proto_PinkSlimeATTACK_Texture");
			break;
		
		case HIT:
			m_pAnumtorCom->Change_Animation(L"Proto_PinkSlimeHIT_Texture");
			break;

		case DIE:
			m_pAnumtorCom->Change_Animation(L"Proto_PinkSlimeDIE_Texture");
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CPinkSlime::Target_Follow(const _float & fTimeDelta)
{
	// 플레이어 따라가기
	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3		vPlayerPos, vPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	_float fDist = D3DXVec3Length(&(vPlayerPos - vPos));

	if (fDist < 8.f)
	{
		m_eCurState = HIT;
		m_pTransCom->Chase_Target(&vPlayerPos, m_fSpeed, fTimeDelta);
	}
	else
	{
		m_eCurState = IDLE;

		// 몬스터 좌우로 이동하기
		_vec3		vRight;
		m_pTransCom->Get_Info(INFO_RIGHT, &vRight);

		m_fTimeAcc += fTimeDelta;
		if (2.f < m_fTimeAcc)
		{
			m_fIdleSpeed *= -1;
			m_fTimeAcc = 0.f;
		}

		D3DXVec3Normalize(&vRight, &vRight);
		m_pTransCom->Move_Pos(&(vRight * m_fIdleSpeed * fTimeDelta));
	}
}
