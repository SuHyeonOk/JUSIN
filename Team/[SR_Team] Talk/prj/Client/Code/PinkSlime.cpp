#include "stdafx.h"
#include "..\Header\PinkSlime.h"

#include "Export_Function.h"

CPinkSlime::CPinkSlime(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_ePreState(MOTION_END)
	, m_eCurState(MOTION_END)
	, m_fTimeAcc(0.f)

{
}


CPinkSlime::~CPinkSlime()
{
}

HRESULT CPinkSlime::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Pos(5.f, 0.f, 25.f);
	m_pTransCom->Set_Scale(m_fScale, m_fScale, m_fScale);

	m_eCurState = IDLE;

	m_fIdle_Speed = 1.f;
	m_fAttack_Speed = 2.f;

	return S_OK;
}

_int CPinkSlime::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	m_pTransCom->Set_Y(m_fHeight);

	m_pAnimtorCom->Play_Animation(fTimeDelta);

	Motion_Change(fTimeDelta);
	Target_Follow(fTimeDelta);
	
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

	m_pAnimtorCom->Set_Texture();
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

	// m_pAnimtorCom
	pComponent = m_pAnimtorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_AnimatorCom"));
	NULL_CHECK_RETURN(m_pAnimtorCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_AnimatorCom", pComponent });

	m_pAnimtorCom->Add_Component(L"Proto_PinkSlimeIDLE_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_PinkSlimeATTACK_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_PinkSlimeHIT_Texture");
	m_pAnimtorCom->Add_Component(L"Proto_PinkSlimeDIE_Texture");

	return S_OK;
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
		m_eCurState = ATTACK;

		Scale_Change(); 
		m_pTransCom->ChangeHeight_Target(&vPlayerPos, m_fHeight, m_fAttack_Speed, fTimeDelta);
	}
	else
		m_eCurState = IDLE;

}

void CPinkSlime::Scale_Change()
{
	_float fSize = 0.f;

	if (Engine::Get_DIKeyState(DIK_P) & 0X80)
	{
		// TODO : 분리 되며 생성되는 PinkSlime
		//CLayer*   pLayer = Engine::Get_Layer(L"Layer_GameLogic");
		//pLayer->Add_GameObject(L"Fist", pGameObject);


		fSize = 0.9f;

		m_pTransCom->Set_Scale(m_fScale * fSize, m_fScale * fSize, m_fScale * fSize);
		m_fHeight = (m_fScale * fSize);
	}
	else if (Engine::Get_DIKeyState(DIK_O) & 0X80)
	{
		fSize = 0.7f;

		m_pTransCom->Set_Scale(m_fScale * fSize, m_fScale * fSize, m_fScale * fSize);
		m_fHeight = (m_fScale * fSize);
	}
	else if (Engine::Get_DIKeyState(DIK_I) & 0X80)
	{
		fSize = 0.5f;

		m_pTransCom->Set_Scale(m_fScale * fSize, m_fScale * fSize, m_fScale * fSize);
		m_fHeight = (m_fScale * fSize);
	}
	else if (Engine::Get_DIKeyState(DIK_U) & 0X80)
	{
		m_pTransCom->Set_Scale(2.f, 2.f, 2.f);
		m_fHeight = (2.f);
	}
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
			m_pAnimtorCom->Change_Animation(L"Proto_PinkSlimeIDLE_Texture");
			break;

		case ATTACK:
			m_pAnimtorCom->Change_Animation(L"Proto_PinkSlimeATTACK_Texture");
			break;
		
		case HIT:
			m_pAnimtorCom->Change_Animation(L"Proto_PinkSlimeHIT_Texture");
			break;

		case DIE:
			m_pAnimtorCom->Change_Animation(L"Proto_PinkSlimeDIE_Texture");
			break;
		}
		m_ePreState = m_eCurState;
	}
}