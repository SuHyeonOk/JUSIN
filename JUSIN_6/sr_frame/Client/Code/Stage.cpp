#include "stdafx.h"
#include "..\Header\Stage.h"

#include "StaticCamera.h"
#include "Terrain.h"
#include "Player.h"
#include "AbstFactory.h"

#include "Particle.h"
#include "ArrowCubeMgr.h"
#include "StatComponent.h"
#include "PlayerUI.h"

//monster
#include "Geomancer.h"
#include "Zombie.h"
#include "Creeper.h"
#include "Skeleton.h"
#include "Enchanter.h"
#include "UI.h"
#include "CoolTimeUI.h"
#include "BatchTool.h"
#include "DamageFontMgr.h"
#include "Trigger.h"
#include "Enderman.h"
#include "Leaper.h"

// object
#include "ObjectStoreMgr.h"
#include "Cat.h"
#include "Cat2.h"
#include "PlayerStartPos.h"
#include "SkyBox.h"
#include "Dynamite.h"
#include "Portrait.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}


CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene(void)
{
	if (FAILED(Engine::CScene::Ready_Scene()))
		return E_FAIL;

	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type		= D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Specular	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Ambient	= D3DXCOLOR(0.6f, 0.6f, 0.5f, 1.f);
	tLightInfo.Direction  = _vec3(0.3f, -1.f, 0.15f);
	m_pGraphicDev->SetLight(0, &tLightInfo);
	m_pGraphicDev->LightEnable(0, TRUE);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	Engine::Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
		->PlayeCamAnimation(L"../Bin/Resource/CubeAnim/Cam/10_12_Done.anim");

	CBatchTool::Load(L"../Bin/Resource/Batch/LASTLASTLASTSTAGE.batch");

	return S_OK;
}

_int CStage::Update_Scene(const _float & fTimeDelta)
{
	if(m_pPlayer != nullptr)
	{
		if (m_pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)->IsDead())
		{
			if (m_fDeadTime > 3.f)
				m_pPlayer->PlayerSpawn();
				
			m_fDeadTime += fTimeDelta;

			if (m_bPlayerAlive)
			{
				m_pPlayerUI = CUIFactory::Create<CPlayerUI>("PlayerUI", L"PlayerDead", 0, WINCX * 0.5f, WINCY * 0.5f, WINCX, WINCY);
				m_pPlayerUI->Open();
				m_pPlayerUI->SetUITexture(25);

				CPlayerStartPos* startPos = Get_GameObjectUnCheck<CPlayerStartPos>(LAYER_GAMEOBJ, L"PlayerPos");
				if (startPos)
				{
					_matrix matWorld = m_pPlayer->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->m_matWorld;
					startPos->Get_Component<CTransform>(L"Proto_TransformCom", ID_DYNAMIC)->Set_WorldDecompose(matWorld);
				}
			}
			m_bPlayerAlive = false;
			
		}
		else
		{
			if (m_pPlayerUI != nullptr)
			{
				m_pPlayerUI->Close();
				m_pPlayerUI = nullptr;
			}
			m_bPlayerAlive = true;
			m_fDeadTime = 0.f;
		}
	}
	
	CSoundMgr::GetInstance()->Update_Listener(LAYER_ENV, L"StaticCamera");
	CDamageFontMgr::GetInstance()->Update_DamageFontMgr(fTimeDelta);
	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CStage::LateUpdate_Scene(void)
{
	Engine::CScene::LateUpdate_Scene();
}

void CStage::Render_Scene(void)
{
	CArrowCubeMgr::GetInst().Render_Buffer();
	CDamageFontMgr::GetInstance()->Render_DamageFontMgr();
}

HRESULT CStage::Ready_Layer_Environment()
{
	CGameObject*		pGameObject = nullptr;

	CEnvFactory::Create<CSkyBox>("SkyBox", L"SkyBox");

	CEnvFactory::Create<CStaticCamera>("StaticCamera", L"StaticCamera");

	// Terrain
	CEnvFactory::Create<CTerrainWater>("WaterTerrain", L"WaterTerrain");

	// TerrainCubeMap
	pGameObject = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/Stage1.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic()
{
	_matrix matWorld;


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 3.6f, 7.f ,3.5f });
	CObjectFactory::Create<CPlayerStartPos>("PlayerPos", L"PlayerPos", matWorld);

	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(90.f) ,0.f }, { 1.f, 0.f ,1.f });

	switch (CObjectStoreMgr::GetInstance()->GetPlayerSkin())
	{
		case Protocol::PLAYER_TYPE_STEVE:
			m_pPlayer = CPlayerFactory::Create<CPlayer>("Steve", L"Player", matWorld);
			break;
		case Protocol::PLAYER_TYPE_PRIDE:
			m_pPlayer = CPlayerFactory::Create<CPlayer>("Pride", L"Player", matWorld);
			break;
		case Protocol::PLAYER_TYPE_ESHE:
			m_pPlayer = CPlayerFactory::Create<CPlayer>("Eshe", L"Player", matWorld);
			break;
		case Protocol::PLAYER_TYPE_COPPER:
			m_pPlayer = CPlayerFactory::Create<CPlayer>("Copper", L"Player", matWorld);
			break;
		case Protocol::PlayerSkin_INT_MIN_SENTINEL_DO_NOT_USE_: break;
		case Protocol::PlayerSkin_INT_MAX_SENTINEL_DO_NOT_USE_: break;
		default: ;
	}
	m_pPlayer->SetName(CObjectStoreMgr::GetInstance()->GetPlayerName());
	// m_pPlayer->PlayerSpawn();


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(90.f) ,0.f }, { 5.75f, 7.f ,10.f });
	CObjectFactory::Create<CCat>("Cat", L"Cat", matWorld);

	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f ,1.f }, { 0.f, D3DXToRadian(90.f), 0.f }, { 33.f, 0.f, 109.f });
	CObjectFactory::Create<CCat2>("Cat2", L"Cat2", matWorld);

	CEffectFactory::Create<C3DBaseTexture>("3D_Base", L"3D_Base");
	CEffectFactory::Create<CAttack_P>("Attack_Basic", L"Attack_Basic");
	CEffectFactory::Create<CFireWork_Fuze>("FireWork_Fuze", L"FireWork_Fuze");
	CEffectFactory::Create<CFireWork>("FireWork", L"FireWork");
	CEffectFactory::Create<CSpeedBoots>("Speed_Boots", L"Speed_Boots");
	CEffectFactory::Create<CSpeedBoots_Particle>("Speed_Boots_Particle", L"Speed_Boots_Particle");

	
	//monsters
	{	
	
		//CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.5f, 1.5f, 1.5f }, { 0.f, 0.f ,0.f }, { 5.f, 4.f, 29.f });
		//CEnemyFactory::Create<CEnderman>("Enderman", L"Enderman", matWorld);


		CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.3f, 1.3f, 1.3f }, { 0.f, D3DXToRadian(180.f) ,0.f }, { 51.f, 4.f, 95.f });
		CEnemyFactory::Create<CLeaper>("Leaper", L"Leaper", matWorld);
	}
	

	CreateTrigger();
	return S_OK;
}

HRESULT CStage::Ready_Layer_UI()
{
	// 플레이어 생성하고 생성하기
	CUIFactory::Create<CUI>("HPUI", L"HPUI", -1, WINCX/2, WINCY - 50, 100, 80);
	CUIFactory::Create<CCoolTimeUI>("PotionCoolTime", L"PotionCoolTime", -1, WINCX/2 + 90, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("RollCoolTime", L"RollCoolTime", -1, WINCX/2 + 140, WINCY - 30, 30, 30);


	CUIFactory::Create<CCoolTimeUI>("Legacy1CoolTime", L"Legacy1CoolTime", -1, WINCX/2 - 210, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("Legacy2CoolTime", L"Legacy2CoolTime", -1, WINCX/2 - 150, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("Legacy3CoolTime", L"Legacy3CoolTime", -1, WINCX/2 - 90, WINCY - 40, 50, 50);

	CUIFactory::Create<CCountUI>("ArrowUI", L"ArrowUI", -1, WINCX/2 + 190, WINCY - 30, 50, 50);
	CUIFactory::Create<CCountUI>("EmeraldUI", L"EmeraldUI", -1, WINCX/2 + 250, WINCY - 30, 20, 30);

	// 플레이어 생성하고 생성하기

	return S_OK;
}



CStage * CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage *	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}


void CStage::Free(void)
{
	CScene::Free();
}

void CStage::CreateTrigger()
{
	_matrix matWorld;
	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 52.f, 4.f, 28.f });

	CTrigger* trigger =  CObjectFactory::Create<CTrigger>("Trigger", L"Trigger1", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				_matrix matWorld;
				_int iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 48.f, 4.f, 42.f });
				CEnemyFactory::Create<CGeomancer>("Geomancer", L"Geomancer", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 54.f, 4.f, 37.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 58.f, 4.f, 30.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 57.f, 4.f, 25.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 55.f, 4.f, 20.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 29.f, 4.f, 17.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);
				
				return true;
			}
		}
		return false;
	}, 6.f);


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 33.f, 4.f, 54.f });

	trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger2", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				_matrix matWorld;
				_int iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 2.f, 2.f, 2.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 36.f, 4.f, 56.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 2.f, 2.f, 2.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 32.f, 4.f, 58.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 2.f, 2.f, 2.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 33.f, 4.f, 50.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);
				return true;
			}
		}
		return false;
	}, 5.f);


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 62.f, 3.f, 84.f });

	trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger3", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				_matrix matWorld;
				_int iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 66.f, 3.f, 89.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 63.f, 3.f, 90.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 61.f, 3.f, 92.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);

				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 56.f, 3.f, 92.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 52.f, 3.f, 91.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 52.f, 4.f, 88.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);

				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 54.f, 3.f, 84.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 57.f, 3.f, 79.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 62.f, 3.f, 79.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);
				return true;
			}
		}
		return false;
	}, 3.f);


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 76.f, 4.f, 114.f });

	trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger4", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				_matrix matWorld;
				_int iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.5f, 1.5f, 1.5f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 70.f, 4.f, 115.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.8f, 1.8f, 1.8f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 81.f, 4.f, 111.f });
				CEnemyFactory::Create<CCreeper>("Creeper", L"Creeper", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 81.f, 4.f, 111.f });
				CEnemyFactory::Create<CEnchanter>("Enchanter", L"Enchanter", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 69.f, 4.f, 118.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 69.f, 4.f, 114.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 68.f, 4.f, 114.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);


				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 68.f, 4.f, 118.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 67.f, 4.f, 118.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 67.f, 4.f, 114.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 66.f, 4.f, 114.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 66.f, 4.f, 118.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 80.f, 4.f, 107.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 80.f, 4.f, 106.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 84.f, 4.f, 107.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 84.f, 4.f, 106.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 2.f, 2.f, 2.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 82.f, 4.f, 105.f });
				CEnemyFactory::Create<CZombie>("Zombie", L"Zombie", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 65.f, 4.f, 116.f });
				CEnemyFactory::Create<CGeomancer>("Geomancer", L"Geomancer", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 85.f, 4.f, 104.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 83.f, 4.f, 104.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);

				iLook = rand() % 361;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(iLook) ,0.f }, { 80.f, 4.f, 104.f });
				CEnemyFactory::Create<CSkeleton>("Skeleton", L"Skeleton", matWorld);
				return true;
			}
		}
		return false;
	}, 3.f);


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.0f, 1.0f, 1.0f }, { 0.f, D3DXToRadian(180.f) ,0.f }, { 95.f, 4.f, 36.f });
	trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger5", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				_matrix matWorld;
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.5f, 1.5f, 1.5f }, { 0.f, 0.f ,0.f }, { 95.f, 6.f, 16.f });
				CEnemyFactory::Create<CEnderman>("Enderman", L"Enderman", matWorld);
				CSoundMgr::GetInstance()->PlayBGM(L"Endermanbgm01.ogg", 0.2f);
				return true;
			}
		}
		return false;
	}, 5.f);


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 45.f, 4.f, 75.f });
	trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger_end", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				CSoundMgr::GetInstance()->PlayBGM(L"madness(leaper)_001.ogg", 0.35f);
				return true;
			}
		}
		return false;
	}, 5.f);


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, 0.f ,0.f }, { 95.f, 6.f, 4.f });
	trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger_end", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				CSceneFactory::LoadScene("Loading1", "Stage_Boss", true, 500);
				return true;
			}
		}
		return false;
	}, 4.f);







}
