#include "stdafx.h"
#include "..\Header\SmileMan.h"

#include "Export_Function.h"

#include "TalkWindow.h"

CSmileMan::CSmileMan(LPDIRECT3DDEVICE9 pGraphicDev)
	:CNPC(pGraphicDev)
{
}


CSmileMan::~CSmileMan()
{
}

HRESULT CSmileMan::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Pos(20.f, 1.f, 3.f);

	// 다이얼로그 테스트 (※메모리 누수 주의)
	wstring str = L"";
	m_vecDialogue.push_back(str);

	str = L"^ - ^";
	m_vecDialogue.push_back(str);

	str = L"^ _ ^ ?";
	m_vecDialogue.push_back(str);

	str = L"뭐해 얼른 내 잘생긴 얼굴을 봐!";
	m_vecDialogue.push_back(str);

	return S_OK;
}

_int CSmileMan::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	m_pTransCom->Set_Y(1.f);
	m_pAnimtorCom->Play_Animation(fTimeDelta, 0.5f);
	
	OnText(fTimeDelta);

	return 0;
}

void CSmileMan::LateUpdate_Object(void)
{
	CNPC::Billboard();
	Engine::CGameObject::LateUpdate_Object();
}

void CSmileMan::Render_Obejct(void)
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

HRESULT CSmileMan::Add_Component(void)
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

	m_pAnimtorCom->Add_Component(L"Proto_NPC_SmileMan_Texture");

	return S_OK;
}

void CSmileMan::OnText(const _float& fTimeDelta)
{
	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(L"Layer_GameLogic", L"Player", L"Proto_TransformCom", ID_DYNAMIC));
	NULL_CHECK(pPlayerTransformCom);

	_vec3 vPlayerPos, vPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	_float fDist = D3DXVec3Length(&(vPlayerPos - vPos));


	m_fLBClick += fTimeDelta;
	if (fDist < 2.f && Engine::Get_DIKeyState(DIK_T) && m_fLBClick > 0.3f) // 거리 안 으로 들어올 때 대화 가능
	{
		CTalkWindow* pTalkWindow = dynamic_cast<CTalkWindow*>(Engine::Get_GameObject(L"Layer_UI", L"UI_TalkWindow"));
		NULL_CHECK(pTalkWindow);

		pTalkWindow->Set_OnText();
		pTalkWindow->Set_TextCount();
		pTalkWindow->Set_Text(&m_vecDialogue);


		m_fLBClick = 0.f;
	}
	//else if(fDist > 2.f || Engine::Get_DIKeyState(DIK_RETURN)) //TODO 대화를 이어서 나가고 싶다면 구현 해야함
	//{
	//	CTalkWindow* pTalkWindow = dynamic_cast<CTalkWindow*>(Engine::Get_GameObject(L"Layer_UI", L"TalkWindow"));
	//	NULL_CHECK(pTalkWindow);

	//	pTalkWindow->Set_OFFText();
	//}
}

CSmileMan * CSmileMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSmileMan *	pInstance = new CSmileMan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSmileMan::Free(void)
{
	CNPC::Free();
}
