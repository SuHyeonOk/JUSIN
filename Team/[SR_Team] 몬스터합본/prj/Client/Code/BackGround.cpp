#include "stdafx.h"
#include "..\Header\BackGround.h"
#include "Export_Function.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}


CBackGround::~CBackGround()
{
}

HRESULT CBackGround::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

_int CBackGround::Update_Object(const _float & fTimeDelta)
{
	// m_pTransCom->m_vScale.x = 2.f;
	// m_pTransCom->m_vScale.y = 2.f;

	// m_pTransCom->m_vAngle.z = D3DXToRadian(45.f);

	CGameObject::Update_Object(fTimeDelta);
	Add_RenderGroup(RENDER_PRIORITY, this); // �ش� �����׷쿡 Background�� �÷ȴ�.

	return 0;
}

void CBackGround::LateUpdate_Object(void)
{
	CGameObject::LateUpdate_Object();
}

void CBackGround::Render_Obejct(void)
{
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	// �ؽ�ó�� ����� ���� �ؽ�ó ���������� �켱������ �ؾ߸� ��
	m_pTextureCom->Set_Texture(0);

	// �״��� ���� ������
	m_pBufferCom->Render_Buffer();
}

HRESULT CBackGround::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// ����� ���� ������Ʈ
	//pComponent = m_pBufferCom = dynamic_cast<CRcCol*>(Clone_Proto(L"Proto_BufferCom"));	// ������Ʈ�� �����Ͽ� ����Ѵ�.																					// ������ ���� ����(���߿� �ε������� ����)
	//NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_BufferCom", pComponent }); // Background�� ���� ������Ʈ ��Ͽ� �ִ´�.(background�� ���� ������Ʈ)
	
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_LogoTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LogoTexture", pComponent });


	return S_OK;
}

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround *	pInstance = new CBackGround(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBackGround::Free(void)
{
	CGameObject::Free();
}
