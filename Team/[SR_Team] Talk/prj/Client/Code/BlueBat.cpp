#include "stdafx.h"
#include "..\Header\BlueBat.h"

#include "Export_Function.h"

#include "StaticCamera.h"

CBlueBat::CBlueBat(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_ePreState(MOTION_END)
	, m_eCurState(MOTION_END)
	, m_bIdle(false)
	, m_fHeight(0.f)
	, m_fJSpeed(0.2f)
	, m_fJSpeed0(0.2f)
	, m_fAccel(0.01f)
	, m_fKnockBackSpeed(0.f)
	, m_fTimeAcc(0.f)
	, m_fJumpTimeAcc(0.f)
	, m_fIdleTimeAcc(0.f)
	, m_fSkillTimeAcc(0.f)
{
}

CBlueBat::~CBlueBat()
{
}

HRESULT CBlueBat::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Pos(10.f, 1.f, 10.f);

	m_eCurState = IDLE;

	m_fHeight = 1.f;
	m_fIdle_Speed = 1.f;
	m_fAttack_Speed = 2.f;
	m_fKnockBackSpeed = 20.f;

	return S_OK;
}

_int CBlueBat::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pAnimtorCom->Play_Animation(fTimeDelta);

	Motion_Change(fTimeDelta);
	Target_Follow(fTimeDelta);
	KnockBack(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);
	
	return 0;
}

void CBlueBat::LateUpdate_Object(void)
{
	CMonster::Billboard();
	Engine::CGameObject::LateUpdate_Object();
}

void CBlueBat::Render_Obejct(void)
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

HRESULT CBlueBat::Add_Component(void)
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

	m_pAnimtorCom->Add_Component(L"Proto_BlueBatIDLE_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_BlueBatATTACK_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_BlueBatHIT_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_BlueBatDIE_Texture");

	return S_OK;
}

void CBlueBat::Target_Follow(const _float & fTimeDelta)
{
	// 플레이어 따라가기
	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3		vPlayerPos, vPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	_float fDist = D3DXVec3Length(&(vPlayerPos - vPos));

	if (fDist < 7.f && fDist > 4.f) // 플레이어를 향해 걸어간다
	{
		m_eCurState = IDLE;

		m_pTransCom->Chase_Target(&vPlayerPos, m_fAttack_Speed, fTimeDelta);
		m_pTransCom->Set_Y(1.f);
	}
	else if (fDist < 4.f && fDist > 1.f) // 공격을 시작한다
	{
		m_fSkillTimeAcc += fTimeDelta;
		if (2.f < m_fSkillTimeAcc)
		{
			m_bIdle = true;
		}
		if (2.1f < m_fSkillTimeAcc)
		{
			m_bIdle = false;
			m_fSkillTimeAcc = 0;
		}

		if (m_bIdle == true)
			m_bJump = true;

		Jump(fTimeDelta);
	}
}

void CBlueBat::Jump(const _float & fTimeDelta)
{
	if (m_bJump)
	{
		_vec3 vPos;
		m_pTransCom->Get_Info(INFO_POS, &vPos);

		if (m_fJumpTimeAcc > 0.3f && m_fHeight >= vPos.y)
		{	
			m_bJump = false;
			m_fJumpTimeAcc = 0.f;
			m_pTransCom->Set_Y(m_fHeight);
			m_fJSpeed = m_fJSpeed0;
		}
		else
		{
			m_eCurState = ATTACK;

			m_fJSpeed -= m_fAccel;
			m_pTransCom->Plus_PosY(m_fJSpeed);
			m_fJumpTimeAcc += 0.01f;

			if (m_pAnimtorCom->Get_Currentframe() >= 7.f && m_pAnimtorCom->Get_Currentframe() < 8.f)
			{
				CStaticCamera* pStaticCamera = dynamic_cast<CStaticCamera*>(Engine::Get_GameObject(L"Layer_Environment", L"StaticCamera"));
				NULL_CHECK(pStaticCamera);
				pStaticCamera->Set_ShakeY();
			}
		}
	}
	else
	{
		m_fIdleTimeAcc += fTimeDelta;
		if (m_fIdleTimeAcc > 0.55f)
		{
			m_eCurState = IDLE;
			m_fIdleTimeAcc = 0.f;
		}
	}
}

void CBlueBat::KnockBack(const _float& fTimeDelta)
{
	if(Engine::Get_DIKeyState(DIK_V) && 0x08)
	{
		if (!m_bKnockBack)
			m_bKnockBack = true;
	}

	if (!m_bKnockBack)
		return;

	CTransform*		pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3 vPos, vTargetLook;
	m_pTransCom->Get_Info(INFO_POS, &vPos);
	pPlayerTransformCom->Get_Info(INFO_LOOK, &vTargetLook);

	if (m_fBTimeDelta > 0.3f && m_fHeight >= vPos.y)
	{
		m_bKnockBack = false;
		m_fBTimeDelta = 0.f;

		m_pTransCom->Set_Y(m_fHeight);
		m_pTransCom->KnockBack_Target(&vTargetLook, m_fKnockBackSpeed, fTimeDelta);
		m_fBSpeed = m_fBSpeed0;
	}
	else
	{
		m_fBSpeed -= m_fBAccel;
		m_pTransCom->Plus_PosY(m_fBSpeed);
		m_fBTimeDelta += 0.1f;
	}
}

void CBlueBat::Motion_Change(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_pAnimtorCom->Change_Animation(L"Proto_BlueBatIDLE_Texture");
			break;

		case ATTACK:
			m_pAnimtorCom->Change_Animation(L"Proto_BlueBatATTACK_Texture");
			break;

		case HIT:
			m_pAnimtorCom->Change_Animation(L"Proto_BlueBatHIT_Texture");
			break;

		case DIE:
			m_pAnimtorCom->Change_Animation(L"Proto_BlueBatDIE_Texture");
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CBlueBat * CBlueBat::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBlueBat *	pInstance = new CBlueBat(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBlueBat::Free(void)
{
	CMonster::Free();
}
