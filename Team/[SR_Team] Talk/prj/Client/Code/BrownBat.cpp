#include "stdafx.h"
#include "..\Header\BrownBat.h"

#include "Export_Function.h"

CBrownBat::CBrownBat(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_ePreState(MOTION_END)
	, m_eCurState(MOTION_END)
	, m_fHeight(0.f)
	, m_fTimeAcc(0.f)
	, m_fAttackTimeAcc(0.f)
{
}

CBrownBat::~CBrownBat()
{
}

HRESULT CBrownBat::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fHeight = 3.f;
	m_pTransCom->Set_Pos(15.f, m_fHeight, 15.f);

	m_eCurState = IDLE;

	m_fIdle_Speed = 2.f;
	m_fAttack_Speed = 10.f;

	return S_OK;
}  

_int CBrownBat::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	m_pAnimtorCom->Play_Animation(fTimeDelta * 3.5f);
	
	Target_Follow(fTimeDelta);
	Motion_Change(fTimeDelta);

	return 0;
}

void CBrownBat::LateUpdate_Object(void)
{
	CMonster::Billboard();
	Engine::CGameObject::LateUpdate_Object();
}

void CBrownBat::Render_Obejct(void)
{
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

HRESULT CBrownBat::Add_Component(void)
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

	m_pAnimtorCom->Add_Component(L"Proto_BrownBatIDLE_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_BrownBatATTACK_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_BrownBatHIT_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_BrownBatDIE_Texture");

	return S_OK;
}

void CBrownBat::Target_Follow(const _float & fTimeDelta)
{
	// 플레이어 따라가기
	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3		vPlayerPos, vPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	_float fDist = D3DXVec3Length(&(vPlayerPos - vPos));

	if (fDist < 10.f)
	{
		m_fAttackTimeAcc += fTimeDelta;
		if (1.f < m_fAttackTimeAcc)
		{
			if (0 < m_fAttack_Speed)
				m_eCurState = IDLE;
			else if (0 > m_fAttack_Speed)
				m_eCurState = ATTACK;

			m_fAttack_Speed *= -1;
			m_fAttackTimeAcc = 0.f;
		}
		m_pTransCom->Chase_Target(&_vec3(vPlayerPos.x, vPlayerPos.y + 2.f, vPlayerPos.z), m_fAttack_Speed, fTimeDelta);
	}
	else
	{
		// 플레이어를 따라다니지 않을 때 좌우 이동
		m_eCurState = IDLE;

		_vec3		vRight;
		m_pTransCom->Get_Info(INFO_RIGHT, &vRight);

		m_fTimeAcc += fTimeDelta;
		if (2.f < m_fTimeAcc)
		{
			m_fIdle_Speed *= -1;
			m_fTimeAcc = 0.f;
		}

		D3DXVec3Normalize(&vRight, &vRight);
		m_pTransCom->Move_Pos(&(vRight * m_fIdle_Speed * fTimeDelta));
	}
}

void CBrownBat::Motion_Change(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_pAnimtorCom->Change_Animation(L"Proto_BrownBatIDLE_Texture");
			break;

		case ATTACK:
			m_pAnimtorCom->Change_Animation(L"Proto_BrownBatATTACK_Texture");
			break;

		case HIT:
			m_pAnimtorCom->Change_Animation(L"Proto_BrownBatHIT_Texture");
			break;

		case DIE:
			m_pAnimtorCom->Change_Animation(L"Proto_BrownBatDIE_Texture");
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CBrownBat * CBrownBat::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBrownBat *	pInstance = new CBrownBat(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBrownBat::Free(void)
{
	CMonster::Free();
}
