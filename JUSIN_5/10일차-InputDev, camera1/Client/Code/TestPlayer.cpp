#include "stdafx.h"
#include "..\Header\TestPlayer.h"

#include "Export_Function.h"

CTestPlayer::CTestPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_vDirection(0.f, 0.f, 0.f)
{
	ZeroMemory(&m_vPlayerDir, sizeof(_vec3));
}


CTestPlayer::~CTestPlayer()
{
}

HRESULT CTestPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTestPlayer::Update_Object(const _float & fTimeDelta)
{
	Key_Input(fTimeDelta);

	Engine::CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CTestPlayer::LateUpdate_Object(void)
{
	

	/*Engine::CGameObject::LateUpdate_Object();*/
}

void CTestPlayer::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTextureCom->Set_Texture(0);	// 텍스처 정보 세팅을 우선적으로 한다.
	m_pBufferCom->Render_Buffer();
	
	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTestPlayer::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_LogoTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LogoTexture", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });


	return S_OK;
}

void CTestPlayer::Key_Input(const _float& fTimeDelta)
{
	float fSpeed = 10.f;

	if (Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransCom->Get_Info(INFO_LOOK, &m_vPlayerDir[0]);

		D3DXVec3Normalize(&m_vPlayerDir[0], &m_vPlayerDir[0]);
		m_pTransCom->Move_Pos(&(m_vPlayerDir[0] * fSpeed * fTimeDelta));
	}

	if (Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransCom->Get_Info(INFO_LOOK, &m_vPlayerDir[0]);

		D3DXVec3Normalize(&m_vPlayerDir[0], &m_vPlayerDir[0]);
		m_pTransCom->Move_Pos(&(m_vPlayerDir[0] * -fSpeed * fTimeDelta));
	}

	if (Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransCom->Get_Info(INFO_RIGHT, &m_vPlayerDir[1]);

		D3DXVec3Normalize(&m_vPlayerDir[1], &m_vPlayerDir[1]);
		m_pTransCom->Move_Pos(&(m_vPlayerDir[1] * -fSpeed * fTimeDelta));
	}

	if (Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransCom->Get_Info(INFO_RIGHT, &m_vPlayerDir[1]);

		D3DXVec3Normalize(&m_vPlayerDir[1], &m_vPlayerDir[1]);
		m_pTransCom->Move_Pos(&(m_vPlayerDir[1] * fSpeed * fTimeDelta));
	}


	//if (GetAsyncKeyState('Q') & 0x8000)
	//	m_pTransCom->Rotation(ROT_X, D3DXToRadian(180.f * fTimeDelta));

	//if (GetAsyncKeyState('A') & 0x8000)
	//	m_pTransCom->Rotation(ROT_X, D3DXToRadian(-180.f * fTimeDelta));

	//if (GetAsyncKeyState('W') & 0x8000)
	//	m_pTransCom->Rotation(ROT_Y, D3DXToRadian(180.f * fTimeDelta));

	//if (GetAsyncKeyState('S') & 0x8000)
	//	m_pTransCom->Rotation(ROT_Y, D3DXToRadian(-180.f * fTimeDelta));

	//if (GetAsyncKeyState('E') & 0x8000)
	//	m_pTransCom->Rotation(ROT_Z, D3DXToRadian(180.f * fTimeDelta));

	//if (GetAsyncKeyState('D') & 0x8000)
	//	m_pTransCom->Rotation(ROT_Z, D3DXToRadian(-180.f * fTimeDelta));

}

CTestPlayer * CTestPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestPlayer *	pInstance = new CTestPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTestPlayer::Free(void)
{
	CGameObject::Free();
}
