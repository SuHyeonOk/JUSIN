#include "stdafx.h"
// #include "..\Header\UItool.h"
//
// #include "DynamicCamera.h"
// #include "ImGuiMgr.h"
//
// CUItool::CUItool(LPDIRECT3DDEVICE9 pGraphicDev)
// 	: Engine::CScene(pGraphicDev)
// {
// 	ZeroMemory(&m_tUITool, sizeof(UiTool));
// }
//
// CUItool::~CUItool()
// {
// }
//
// HRESULT CUItool::Ready_Scene(void)
// {
// 	if (FAILED(Engine::CScene::Ready_Scene()))
// 		return E_FAIL;
//
// 	FAILED_CHECK_RETURN(Ready_Proto(), E_FAIL);
// 	FAILED_CHECK_RETURN(Ready_Layer_Environment(), E_FAIL);
//
// 	return S_OK;
// }
//
// _int CUItool::Update_Scene(const _float & fTimeDelta)
// {
// 	IM_BEGIN("UI Editor Window");
//
// 	CImGuiMgr::UiEditor(m_tUITool,m_pCubeMap);
//
// 	IM_END;
//
// 	return Engine::CScene::Update_Scene(fTimeDelta);
// }
//
// void CUItool::LateUpdate_Scene(void)
// {
// 	Engine::CScene::LateUpdate_Scene();
// }
//
// void CUItool::Render_Scene(void)
// {
// }
//
//
// HRESULT CUItool::Ready_Layer_Environment()
// {
// 	CGameObject*		pGameObject = nullptr;
//
// 	pGameObject = m_pDCamera = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f), D3DXToDegree(60.f), (_float)WINCX / WINCY, 0.f, 1000.f);
// 	NULL_CHECK_RETURN(pGameObject, E_FAIL);
// 	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);
// 	g_cam = m_pDCamera;
//
// 	/*pGameObject = CTerrain::Create(m_pGraphicDev);
// 	NULL_CHECK_RETURN(pGameObject, E_FAIL);
// 	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"Terrain", pGameObject), E_FAIL);*/
// 	
// 	return S_OK;
// }
//
// HRESULT CUItool::Ready_Proto(void)
// {
// 	// FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexCom", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
// 	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformCom", CTransform::Create()), E_FAIL);
// 	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UI_Texture", CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_%d.png", TEX_NORMAL, 14)), E_FAIL);
//
//
// 	return S_OK;
// }
//
// CUItool * CUItool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
// {
// 	CUItool*		pInstance = new CUItool(pGraphicDev);
//
// 	if (FAILED(pInstance->Ready_Scene()))
// 	{
// 		Safe_Release(pInstance);
// 		return nullptr;
// 	}
//
// 	return pInstance;
// }
//
// void CUItool::Free(void)
// {
// 	CScene::Free();
// }
//
