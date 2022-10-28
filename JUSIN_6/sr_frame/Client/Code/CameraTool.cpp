#include "stdafx.h"
#include "CameraTool.h"

#include "AbstFactory.h"
#include "DynamicCamera.h"
#include "TestCube.h"
#include "ImGuiMgr.h"
#include "SkeletalCube.h"
#include "StaticCamera.h"
#include "TerrainCubeMap.h"
#include "RedStoneMonstrosity.h"

CCameraTool::CCameraTool(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}

CCameraTool::~CCameraTool()
{
}

HRESULT CCameraTool::Ready_Scene()
{
	CScene::Ready_Scene();

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

	pGameObject = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/Stage2.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);



	_matrix matWorld;
	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.5f, 1.5f, 1.5f }, { 0.f, D3DXToRadian(250.f) ,0.f }, { 109.f, 0.54f, 14.f });
	CEnemyFactory::Create<CRedStoneMonstrosity>("RedStoneMonstrosity", L"RedStoneMonstrosity", matWorld);

	CEnvFactory::Create<CStaticCamera>("StaticCamera", L"StaticCamera");

	return S_OK;
}

_int CCameraTool::Update_Scene(const _float& fTimeDelta)
{
	
	IM_BEGIN("Skeletal Editor");
	CImGuiMgr::SkeletalEditor(m_pCam, m_Skel);
	IM_END;

	// ImGui::SetNextWindowSize(ImVec2(960, 300));
	IM_BEGIN("Camera Editor");
	CImGuiMgr::AnimationEditor(m_Skel);
	IM_END;
	// CTransform*	pDynamicCamTransform = Engine::Get_Component<CTransform>(LAYER_ENV, L"DynamicCamera", L"Proto_TransformCom", ID_DYNAMIC);

	/*CTransform*	pStaticCamTransform = Engine::Get_Component<CTransform>(LAYER_ENV, L"StaticCamera", L"Proto_TransformCom", ID_DYNAMIC);*/
	
	IM_BEGIN("cam");

	if (ImGui::Button("Play Anim"))
	{


		Engine::Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayeCamAnimation(L"../Bin/Resource/CubeAnim/Cam/redston2.anim");
		m_pCam->m_bStop = true;
	}

	IM_END;

	list<CGameObject*> objList;
	Get_AllGameObject(LAYER_GAMEOBJ, L"CamAnim", OUT objList);
	if (objList.empty())
	{
		m_pCam->m_bStop = false;
	}

	return CScene::Update_Scene(fTimeDelta);
}

void CCameraTool::Free()
{
	CScene::Free();
}

CCameraTool* CCameraTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraTool*	pInstance = new CCameraTool(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}


	return pInstance;
}


