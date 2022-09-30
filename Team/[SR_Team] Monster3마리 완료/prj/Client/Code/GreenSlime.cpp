#include "stdafx.h"
#include "..\Header\GreenSlime.h"

#include "Export_Function.h"

CGreenSlime::CGreenSlime(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
	, m_ePreState(MOTION_END)
	, m_eCurState(MOTION_END)
	, m_fTimeAcc(0.f)

{
}


CGreenSlime::~CGreenSlime()
{
}

HRESULT CGreenSlime::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Set_Pos(0.f, 1.f, 20.f);

	m_eCurState = IDLE;

	m_fIdle_Speed = 1.f;
	m_fAttack_Speed = 2.f;

	return S_OK;
}

_int CGreenSlime::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	m_pTransCom->Set_Y(1.f);

	m_pAnumtorCom->Play_Animation(fTimeDelta);
	//// 애니메이션 변화
	//m_fFrame += m_pTextureCom->Get_FrameEnd()  * fTimeDelta;

	//if (m_fFrame >= m_pTextureCom->Get_FrameEnd())
	//	m_fFrame = 0;
	
	Target_Follow(fTimeDelta);
	Motion_Change(fTimeDelta);

	return 0;
}

void CGreenSlime::LateUpdate_Object(void)
{
	CMonster::Billboard();
	Engine::CGameObject::LateUpdate_Object();
}

void CGreenSlime::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pAnumtorCom->Set_Texture();
	//m_pTextureCom->Set_Texture((_ulong)m_fFrame);	// 텍스처 정보 세팅을 우선적으로 한다.
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CGreenSlime::Add_Component(void)
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

	m_pAnumtorCom->Add_Component(L"Proto_GreenSlimeIDLE_Texture");
	m_pAnumtorCom->Add_Component(L"Proto_GreenSlimeATTACK_Texture");
	m_pAnumtorCom->Add_Component(L"Proto_GreenSlimeHIT_Texture");
	m_pAnumtorCom->Add_Component(L"Proto_GreenSlimeDIE_Texture");

	//// m_pTextureCom	
	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GreenSlimeIDLE_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_GreenSlimeIDLE_Texture", pComponent });

	//pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GreenSlimeATTACK_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_GreenSlimeATTACK_Texture", pComponent });

	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GreenSlimeHIT_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_GreenSlimeHIT_Texture", pComponent });

	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GreenSlimeDIE_Texture"));
	//NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_GreenSlimeDIE_Texture", pComponent });
	/////////

	return S_OK;
}

void CGreenSlime::Target_Follow(const _float & fTimeDelta)
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
		m_pTransCom->Chase_Target(&vPlayerPos, m_fAttack_Speed, fTimeDelta);
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
			m_fIdle_Speed *= -1;
			m_fTimeAcc = 0.f;
		}

		D3DXVec3Normalize(&vRight, &vRight);
		m_pTransCom->Move_Pos(&(vRight * m_fIdle_Speed * fTimeDelta));

		//////
		//_vec3		vSenter = { 15.f, 0.f, 15.f };

		//_float fDist = D3DXVec3Length(&(vSenter - vPos));

		// _float		fRadius = 5.f;

		//if (fDist > fRadius)
		//{
		//	m_fIdleSpeed *= -1;
		//}	

		//D3DXVec3Normalize(&vRight, &vRight);
		//m_pTransCom->Move_Pos(&(vRight * m_fIdleSpeed * vRight));
	}
}
void CGreenSlime::Motion_Change(const _float& fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_pAnumtorCom->Change_Animation(L"Proto_GreenSlimeIDLE_Texture");
			//m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_GreenSlimeIDLE_Texture", ID_STATIC));
			break;

		case ATTACK:
			m_pAnumtorCom->Change_Animation(L"Proto_GreenSlimeATTACK_Texture");
			//m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_GreenSlimeATTACK_Texture", ID_STATIC));
			break;

		case HIT:
			m_pAnumtorCom->Change_Animation(L"Proto_GreenSlimeHIT_Texture");
			//m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_GreenSlimeHIT_Texture", ID_STATIC));
			break;

		case DIE:
			m_pAnumtorCom->Change_Animation(L"Proto_GreenSlimeDIE_Texture");
			//m_pTextureCom = static_cast<CTexture*>(Find_Component(L"Proto_GreenSlimeDIE_Texture", ID_STATIC));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CGreenSlime * CGreenSlime::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGreenSlime *	pInstance = new CGreenSlime(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CGreenSlime::Free(void)
{
	CMonster::Free();
}
