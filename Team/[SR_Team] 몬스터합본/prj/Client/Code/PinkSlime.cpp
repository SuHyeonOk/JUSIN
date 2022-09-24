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

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pAnimater->Play_Animation(fTimeDelta);

	//// 애니메이션 변화
	//m_fFrame += m_pTextureCom->Get_FrameEnd()  * fTimeDelta;

	//if (m_fFrame >= m_pTextureCom->Get_FrameEnd())
	//	m_fFrame = 0;

	//Motion_Change(fTimeDelta);

	//Target_Follow(fTimeDelta);
	Billboard();

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CPinkSlime::LateUpdate_Object(void)
{
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

	m_pAnimater->Set_Text();
	//m_pTextureCom->Set_Texture((_ulong)m_fFrame);	// 텍스처 정보 세팅을 우선적으로 한다.
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

	// m_pAnimater
	pComponent = m_pAnimater = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_AnimatorCom"));
	NULL_CHECK_RETURN(m_pAnimater, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_AnimatorCom", pComponent });

	m_pAnimater->Add_Com(L"Proto_PinkSlimeIDLE_Texture", ID_STATIC);


	//// m_pTextureCom	
	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PinkSlimeIDLE_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_PinkSlimeIDLE_Texture", pComponent });

	//pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PinkSlimeATTACK_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_PinkSlimeATTACK_Texture", pComponent });

	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PinkSlimeHIT_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_PinkSlimeHIT_Texture", pComponent });

	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PinkSlimeDIE_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_PinkSlimeDIE_Texture", pComponent });
	///////

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

	if (fDist < 10.f)
	{
		m_eCurState = ATTACK;
		m_pTransCom->Chase_Target(&vPlayerPos, m_fSpeed, fTimeDelta);
	}
	else
	{
		m_eCurState = IDLE;
	}
}

void CPinkSlime::Billboard()
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

	// 현재 지금 이 코드는 문제가 없지만 나중에 문제가 될 수 있음
	m_pTransCom->Set_WorldMatrix(&(matBill * matWorld));
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
	CGameObject::Free();
}

void CPinkSlime::Motion_Change(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_PinkSlimeIDLE_Texture", ID_STATIC));
			break;

		case ATTACK:
			m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_PinkSlimeATTACK_Texture", ID_STATIC));
			break;

		case HIT:
			m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_PinkSlimeHIT_Texture", ID_STATIC));
			break;

		case DIE:
			m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_PinkSlimeDIE_Texture", ID_STATIC));
			break;
		}
		m_ePreState = m_eCurState;
	}
}
