#include "stdafx.h"
#include "AnimationTool.h"
#include "DynamicCamera.h"
#include "TestCube.h"
#include "ImGuiMgr.h"
#include "SkeletalCube.h"
#include "TerrainCubeMap.h"

CAnimationTool::CAnimationTool(LPDIRECT3DDEVICE9 pGraphicDev): CScene(pGraphicDev)
{
}

CAnimationTool::~CAnimationTool()
{
}

HRESULT CAnimationTool::Ready_Scene()
{
	CGameObject*		pGameObject = nullptr;

	// DynamicCamera
	pGameObject = m_pCam = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_GAMEOBJ]->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);
		
	// skeletal
	pGameObject = m_Skel = CSkeletalCube::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_GAMEOBJ]->Add_GameObject(L"Skeletal", pGameObject), E_FAIL);
	// m_pSelectedTransform = dynamic_cast<CTestCube*>(pGameObject)->m_pTransCom;

	pGameObject = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/koukusatonbase.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);

	return CScene::Ready_Scene();
}

_int CAnimationTool::Update_Scene(const _float& fTimeDelta)
{
	// ImGui::ShowDemoWindow(nullptr);

	IM_BEGIN("Skeletal Editor");
	CImGuiMgr::SkeletalEditor(m_pCam, m_Skel);
	IM_END;

	// ImGui::SetNextWindowSize(ImVec2(960, 300));
	IM_BEGIN("Animation Editor");
	CImGuiMgr::AnimationEditor(m_Skel);
	IM_END;

	return CScene::Update_Scene(fTimeDelta);
}

void CAnimationTool::Free()
{
	CScene::Free();
}

CAnimationTool* CAnimationTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAnimationTool*	pInstance = new CAnimationTool(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}


	return pInstance;
}

