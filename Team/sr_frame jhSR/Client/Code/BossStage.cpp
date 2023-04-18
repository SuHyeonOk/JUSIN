#include "stdafx.h"
#include "..\Header\BossStage.h"
#include "AbstFactory.h"
#include "StaticCamera.h"
#include "Terrain.h"
#include "Player.h"
#include "ObjectStoreMgr.h"
#include "Trigger.h"
#include "MapTable.h"
#include "CoolTimeUI.h"
#include "DamageFontMgr.h"
#include "ArrowCubeMgr.h"
#include "TerrainCubeMap.h"
#include "RedStoneMonstrosity.h"
#include "StatComponent.h"
#include "PlayerUI.h"


CBossStage::CBossStage(LPDIRECT3DDEVICE9 pGraphicDev) : CScene(pGraphicDev)
{
}


CBossStage::~CBossStage()
{
}

HRESULT CBossStage::Ready_Scene(void)
{
	if (FAILED(Engine::CScene::Ready_Scene()))
		return E_FAIL;

	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type		= D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Specular	= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	tLightInfo.Ambient	= D3DXCOLOR(0.6f, 0.6f, 0.3f, 1.f);
	tLightInfo.Direction  = _vec3(0.3f, -1.f, 0.15f);
	m_pGraphicDev->SetLight(0, &tLightInfo);
	m_pGraphicDev->LightEnable(0, TRUE);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(), E_FAIL);

	return S_OK;
}

_int CBossStage::Update_Scene(const _float & fTimeDelta)
{
	if (m_pPlayer != nullptr)
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

void CBossStage::LateUpdate_Scene(void)
{
	Engine::CScene::LateUpdate_Scene();
}

void CBossStage::Render_Scene(void)
{
	CArrowCubeMgr::GetInst().Render_Buffer();
	CDamageFontMgr::GetInstance()->Render_DamageFontMgr();
}

HRESULT CBossStage::Ready_Layer_Environment()
{
	CGameObject*		pGameObject = nullptr;

	CEnvFactory::Create<CStaticCamera>("StaticCamera", L"StaticCamera");

	// Terrain
	CEnvFactory::Create<CTerrainWater>("WaterTerrain", L"WaterTerrain");

	// TerrainCubeMap
	pGameObject = CTerrainCubeMap::Create(m_pGraphicDev, L"../Bin/Resource/Map/Stage2.map");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(m_arrLayer[LAYER_ENV]->Add_GameObject(L"TerrainCubeMap", pGameObject), E_FAIL);

	return S_OK;
}

HRESULT CBossStage::Ready_Layer_GameLogic()
{
	_matrix matWorld;

	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.f, 1.f, 1.f }, { 0.f, D3DXToRadian(90.f) ,0.f }, { 66.f, 1.64f, 6.68f });

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
	default:;
	}
	m_pPlayer->SetName(CObjectStoreMgr::GetInstance()->GetPlayerName());
	m_pPlayer->PlayerSpawn();

	CEffectFactory::Create<C3DBaseTexture>("3D_Base", L"3D_Base");
	CEffectFactory::Create<CAttack_P>("Attack_Basic", L"Attack_Basic");
	CEffectFactory::Create<CFireWork_Fuze>("FireWork_Fuze", L"FireWork_Fuze");
	CEffectFactory::Create<CFireWork>("FireWork", L"FireWork");
	CEffectFactory::Create<CSpeedBoots>("Speed_Boots", L"Speed_Boots");
	CEffectFactory::Create<CSpeedBoots_Particle>("Speed_Boots_Particle", L"Speed_Boots_Particle");


	CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.0f, 1.0f, 1.0f }, { 0.f, D3DXToRadian(180.f) ,0.f }, { 85.f, 2.5f, 7.f });
	CTrigger* trigger = CObjectFactory::Create<CTrigger>("Trigger", L"Trigger5", matWorld);
	trigger->SetTrigger([](set<CGameObject*>& objSet) {
		for (auto obj : objSet)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{

				_matrix matWorld;
				//CreateBoss
				CGameUtilMgr::MatWorldComposeEuler(matWorld, { 1.5f, 1.5f, 1.5f }, { 0.f, D3DXToRadian(250.f) ,0.f }, { 109.f, 0.54f, 14.f });
				CEnemyFactory::Create<CRedStoneMonstrosity>("RedStoneMonstrosity", L"RedStoneMonstrosity", matWorld);

				//ActionCam Start
				Engine::Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
					->PlayeCamAnimation(L"../Bin/Resource/CubeAnim/Cam/redston.anim");

				CSoundMgr::GetInstance()->PlayBGM(L"OST_Commander_of_Beast_Valtan_LOST_ARK_Official_Soundtrack.ogg", 0.3f);
				return true;
			}
		}
		return false;
	}, 3.f);

	return S_OK;
}

HRESULT CBossStage::Ready_Layer_UI()
{
	// 플레이어 생성하고 생성하기
	CUIFactory::Create<CUI>("HPUI", L"HPUI", -1, WINCX / 2, WINCY - 50, 100, 80);
	CUIFactory::Create<CCoolTimeUI>("PotionCoolTime", L"PotionCoolTime", -1, WINCX / 2 + 90, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("RollCoolTime", L"RollCoolTime", -1, WINCX / 2 + 140, WINCY - 30, 30, 30);


	CUIFactory::Create<CCoolTimeUI>("Legacy1CoolTime", L"Legacy1CoolTime", -1, WINCX / 2 - 210, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("Legacy2CoolTime", L"Legacy2CoolTime", -1, WINCX / 2 - 150, WINCY - 40, 50, 50);
	CUIFactory::Create<CCoolTimeUI>("Legacy3CoolTime", L"Legacy3CoolTime", -1, WINCX / 2 - 90, WINCY - 40, 50, 50);

	CUIFactory::Create<CCountUI>("ArrowUI", L"ArrowUI", -1, WINCX / 2 + 190, WINCY - 30, 50, 50);
	CUIFactory::Create<CCountUI>("EmeraldUI", L"EmeraldUI", -1, WINCX / 2 + 250, WINCY - 30, 20, 30);
	return S_OK;
}

CBossStage * CBossStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossStage* pInstance = new CBossStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CBossStage::Free(void)
{
	CScene::Free();
}
