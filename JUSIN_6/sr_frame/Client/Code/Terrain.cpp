#include "stdafx.h"
#include "..\Header\Terrain.h"


#pragma region CTerrain
CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
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

void CTerrain::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	 m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pTextureCom->Set_Texture(0);	// 텍스처 정보 세팅을 우선적으로 한다.
	 m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CTerrain::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Clone_Proto(L"Proto_TerrainTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TerrainTexCom", pComponent });
	// m_pBufferCom = Add_Component<CTerrainTex>(L"Proto_TerrainTexCom", L"Proto_TerrainTexCom", ID_STATIC);

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_TerrainTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TerrainTexture", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });


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

#pragma endregion


#pragma region CTerrainWater



CTerrainWater::CTerrainWater(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTerrainWater::~CTerrainWater()
{
}

HRESULT CTerrainWater::Ready_Object()
{

	m_pBufferCom = Add_Component<CTerrainShader>(L"Proto_Terrain_WaterCom", L"Proto_Terrain_WaterCom", ID_STATIC);
	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_Terrain_Water_Texture", L"Proto_Terrain_Water_Texture", ID_STATIC);

	m_pBufferCom->Set_Texture(m_pTextureCom->GetDXTexture());
	m_pBufferCom->Set_TextureOption(0, 0, 0);

	m_pBufferCom->Set_UVOption(CGameUtilMgr::s_fTimeDelta * 1, 1.64f, 0.5f,0.5f, 0.3f);

	// CTransform*	pPlayerTransform = Engine::Get_Component<CTransform>(LAYER_PLAYER, L"Player", L"Proto_TransformCom", ID_DYNAMIC);
	// _vec3 pPos;
	// pPlayerTransform->Get_Info(INFO_POS, &pPos);
	m_pTransCom->m_vInfo[INFO_POS] = _vec3(- 100.f, - 10.f, - 100.f);
	m_pTransCom->m_vScale *= 4.f;









	m_pTransCom->Update_Component(0.f);




	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	return S_OK;
}

_int CTerrainWater::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	// m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;
	// static float timeSpeed = 1.f;
	// m_pBufferCom->m_fTime += fTimeDelta * timeSpeed;
	// IM_BEGIN("test");
	//
	// ImGui::SliderFloat("time", &timeSpeed, 0.f, 100.f);
	// ImGui::SliderFloat("m_fSpeed", &m_pBufferCom->m_fSpeed, 0.f, 100.f);
	// ImGui::SliderFloat("m_fWaveHeight", &m_pBufferCom->m_fWaveHeight, 0.f, 100.f);
	// ImGui::SliderFloat("m_fWaveFreq", &m_pBufferCom->m_fWaveFreq, 0.f, 100.f);
	// ImGui::SliderFloat("m_fUVSpeed", &m_pBufferCom->m_fUVSpeed, 0.f, 100.f);
	//
	// ImGui::InputFloat("time input", &timeSpeed);
	// ImGui::InputFloat("m_fSpeed input", &m_pBufferCom->m_fSpeed);
	// ImGui::InputFloat("m_fWaveHeight input", &m_pBufferCom->m_fWaveHeight);
	// ImGui::InputFloat("m_fWaveFreq input", &m_pBufferCom->m_fWaveFreq);
	// ImGui::InputFloat("m_fUVSpeed input", &m_pBufferCom->m_fUVSpeed);
	//
	// IM_END;
	// m_pTransCom->m_vScale *= 4.f;
	// m_pTransCom->m_vInfo[INFO_POS] = _vec3(- 100.f, - 10.f, - 100.f);

	//IM_LOG("%f, %f, %f", m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].z, m_pTransCom->m_vScale);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CTerrainWater::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CTerrainWater::Render_Object()
{
	m_pBufferCom->Render_Buffer();

	// m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}



CTerrainWater* CTerrainWater::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrainWater *	pInstance = new CTerrainWater(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CTerrainWater::Free()
{
	CGameObject::Free();
}
#pragma endregion

