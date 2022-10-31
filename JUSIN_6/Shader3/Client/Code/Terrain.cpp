#include "stdafx.h"
#include "..\Header\Terrain.h"

#include "Export_Function.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_vDirection({ 0.f, 0.f, 1.f })
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


	Engine::CGameObject::Update_Object(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CTerrain::LateUpdate_Object(void)
{
	Engine::CGameObject::LateUpdate_Object();
}

void CTerrain::Render_Obejct(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	// m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	// m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// m_pTextureCom->Set_Texture(0);	// 텍스처 정보 세팅을 우선적으로 한다.

	FAILED_CHECK_RETURN(SetUp_Material(), );

	_matrix			WorldMatrix, ViewMatrix, ViewMatrixInv, ProjMatrix;

	m_pTransCom->Get_WorldMatrix(&WorldMatrix);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrix);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_matrix));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_matrix));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_matrix));
	m_pShaderCom->Set_RawValue("g_vCamPosition", (_vec3*)&ViewMatrixInv.m[3][0], sizeof(_vec3));

	m_pTextureCom->Set_Texture(m_pShaderCom, "g_DefaultTexture", 0);

	m_pShaderCom->Begin_Shader(0);

	m_pBufferCom->Render_Buffer();

	m_pShaderCom->End_Shader();

	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
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

	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Clone_Proto(L"Proto_ShaderTerrain"));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_ShaderCom", pComponent });


	return S_OK;
}


HRESULT CTerrain::SetUp_Material(void)
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	// 램버트 확산 조명 공식
	// 명도의 세기 * (L.D * M.D) + L.A * M.A


	return S_OK;
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
