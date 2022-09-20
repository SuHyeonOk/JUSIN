#include "stdafx.h"
#include "..\Header\CubePlayer.h"

#include "Export_Function.h"
#include "Bullet.h"

CCubePlayer::CCubePlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fSpeed(0.f)
{
	ZeroMemory(&m_vDirection, sizeof(_vec3));
}

CCubePlayer::~CCubePlayer()
{
}

HRESULT CCubePlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 10.f;


	return S_OK;
}

_int CCubePlayer::Update_Object(const _float & fTimeDelta)
{
	Key_Input(fTimeDelta);

	CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);
	return 0;
}

void CCubePlayer::LateUpdate_Object(void)
{
	Engine::CGameObject::LateUpdate_Object();
}

void CCubePlayer::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	
	m_pTextureCom->Set_Texture(1);
	m_pBufferCom->Render_Buffer();

}

HRESULT CCubePlayer::Add_Component()
{
	CComponent*	pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTexCom", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CubeTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTexture", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	return S_OK;
}

void CCubePlayer::Key_Input(const _float & fTimeDelta)
{
	m_pTransCom->Get_Info(INFO_LOOK, &m_vDirection);

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * m_fSpeed * fTimeDelta));
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * -m_fSpeed * fTimeDelta));
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_pTransCom->Rotation(ROT_Y, D3DXToRadian(180.f * fTimeDelta));

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_pTransCom->Rotation(ROT_Y, D3DXToRadian(-180.f * fTimeDelta));

	// 총알 발사
 	if (Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		Engine::CLayer*		pLayer = Engine::CLayer::Create();
		NULL_CHECK(pLayer, E_FAIL);

		CGameObject*		pGameObject = nullptr;

		pGameObject = CBullet::Create(m_pGraphicDev);
		NULL_CHECK(pGameObject, E_FAIL);
		FAILED_CHECK(pLayer->Add_GameObject(L"Bullet", pGameObject), E_FAIL);
		
		

	}
}

//template<typename T>
//CGameObject* CCubePlayer::Create_Bullet(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vDir)
//{
//	CObj*	pBullet = new CBullet;
//
//	pBullet->Set_Pos(_vPos);
//	pBullet->Set_Dir(_vDir);
//
//	pBullet->Initialize();
//
//
//	return pBullet;
//}

CCubePlayer * CCubePlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCubePlayer*	pInstance = new CCubePlayer(pGraphicDev);

	// 객체의 Ready_Object()함수를 호출하지 못 했다면
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance); // 삭제
		return nullptr;
	}
	// else 호출 했다면 객체 리턴
	return pInstance;
}

void CCubePlayer::Free()
{
	CGameObject::Free();
}
