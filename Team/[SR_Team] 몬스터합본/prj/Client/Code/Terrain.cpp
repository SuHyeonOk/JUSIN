#include "stdafx.h"
#include "..\Header\Terrain.h"

#include "Export_Function.h"	

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev), m_vDirection({ 0.f, 0.f, 1.f }) // ����
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTerrain::Update_Object(const _float & fTimeDelta)
{
	// Key_Input(fTimeDelta);
	CGameObject::Update_Object(fTimeDelta);
	Add_RenderGroup(RENDER_NONALPHA, this); // �����׷쿡 terrain �߰�

	return 0;
}

void CTerrain::LateUpdate_Object(void)
{
	CGameObject::LateUpdate_Object();
}

void CTerrain::Render_Obejct(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	// m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTextureCom->Set_Texture(0);

	FAILED_CHECK_RETURN(SetUp_Material(), ); // �ݵ�� render buffer �տ��� �������� ����
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTerrain::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Proto(L"Proto_TerrainTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TerrainTexCom", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_TerrainTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TerrainTexture", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });


	return S_OK;
}

HRESULT CTerrain::SetUp_Material(void)
{
	// ����� ������ ��갪���� ��µ�(���� �ִٰ� ��µ��� ����)
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

		// ������ rgba ������ 1 �ִ�
	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);	// ������ �߱�(�ڿ��߱�)
	tMtrl.Power = 0.f; // ���ݻ�(�Ի簢�� ���ϴ� �ݻ簢�� ����) Ŀ���� ���̶���Ʈ ũ�Ⱑ �۾���.
						// ������ ���ݻ�� ���� ����(CPU�� �����ؾ� ��)

	m_pGraphicDev->SetMaterial(&tMtrl);

	// ����Ʈ Ȯ�� ���� ����
	// ���� ���� * (L.D * M.D) + L.A * M.A


	return S_OK;
}

void CTerrain::Key_Input(const _float & fTimeDelta)
{
	m_pTransCom->Get_Info(INFO_LOOK, &m_vDirection);


	if (Get_DIKeyState(DIK_W) & 0x80)
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * 10.f * fTimeDelta));
	}
	if (Get_DIKeyState(DIK_S) & 0x80)
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * -10.f * fTimeDelta));
	}

	/*
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * 10.f * fTimeDelta));
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * -10.f * fTimeDelta));
	}

	if (GetAsyncKeyState('Q') & 0x8000)
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState('A') & 0x8000)
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(-90.f * fTimeDelta));

	if (GetAsyncKeyState('W') & 0x8000)
		m_pTransCom->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState('S') & 0x8000)
		m_pTransCom->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));

	if (GetAsyncKeyState('E') & 0x8000)
		m_pTransCom->Rotation(ROT_Z, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState('D') & 0x8000)
		m_pTransCom->Rotation(ROT_Z, D3DXToRadian(-90.f * fTimeDelta));
		*/
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain *	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTerrain::Free(void)
{
	CGameObject::Free();

}
